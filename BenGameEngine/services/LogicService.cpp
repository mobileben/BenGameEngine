//
//  LogicService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/24/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LogicService.h"

#include "Game.h"
#include "GameObject.h"
#include "LogicComponent.h"
#include "Space.h"
#ifdef SUPPORT_PROFILING
#include "Profiling.h"
#endif /* SUPPORT_PROFILING */
BGE::LogicService::LogicService() :  Service() {
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK)) {
        assert(false);
    }
    
    if (pthread_mutex_init(&addRemoveMutex_, &attr)) {
        assert(false);
    }
    
    if (pthread_mutex_init(&executeMutex_, &attr)) {
        assert(false);
    }
}

void BGE::LogicService::update(double deltaTime) {
    addRemoveLock();

#ifdef SUPPORT_PROFILING
    auto startTime = profiling::EpochTime::timeInMicroSec();
    numProcessedObjects_ = 0;
#endif /* SUPPORT_PROFILING */

    // Update our list based on our array
    for (auto &element : addRemoveQueue_) {
        auto item = std::make_pair(element.spaceHandle, element.gameObjHandle);
        
        if (element.op == Operation::Add) {
            gameObjectHandles_.push_back(item);
        } else {
            auto removeHandle = element.gameObjHandle;
            
            for (auto it=gameObjectHandles_.begin();it!=gameObjectHandles_.end();it++) {
                if (it->second == removeHandle) {
                    gameObjectHandles_.erase(it);
                    break;
                }
            }
        }
    }
    
    addRemoveQueue_.clear();
    
    addRemoveUnlock();
    
    executeLock();
    
    std::vector<std::pair<SpaceHandle, GameObjectHandle>> executeQueue;
    
    auto spaceService = Game::getInstance()->getSpaceService();
    auto paused = Game::getInstance()->isPaused();

    spaceService->lock();
    
    // Iterate so we can remove nullptr objects
    for (auto it=gameObjectHandles_.begin();it!=gameObjectHandles_.end();) {
        auto spaceHandle = it->first;
        auto gameObjHandle = it->second;
        auto space = spaceService->getSpace(spaceHandle);
        auto remove = true;
        
        if (space) {
            auto gameObj = space->getGameObject(gameObjHandle);
            
            if (gameObj) {
                auto logic = gameObj->getComponent<LogicComponent>(space);

                if (logic && logic->update) {
                    // As long as we have a valid gameObj and logic ptr and an update, then don't remove
                    remove = false;
                    if (gameObj->isActive()) {
                        if (logic->update) {
                            if (logic->mode == LogicComponentMode::Always || (paused && logic->mode == LogicComponentMode::Paused) || logic->mode == LogicComponentMode::Unapaused) {
                                executeQueue.push_back(std::make_pair(spaceHandle, gameObjHandle));
                            }
                        }
                    }
                }
                // If logic is no longer valid or no update function, this will be removed
            }
            // No longer valid game objects get removed
        }
        
        if (remove) {
            it = gameObjectHandles_.erase(it);
        } else {
            it++;
        }
    }
    
    executeUnlock();
    
    for (auto &item : executeQueue) {
        // Any of these items can be destroyed, so dereference
        auto space = spaceService->getSpace(item.first);
        
        if (space) {
            auto gameObj = space->getGameObject(item.second);
            
            if (gameObj) {
                auto logic = gameObj->getComponent<LogicComponent>(space);
                
                if (logic) {
                    logic->update(gameObj, deltaTime);
#ifdef SUPPORT_PROFILING
                    ++numProcessedObjects_;
#endif /* SUPPORT_PROFILING */
                }
            }
        }
    }

#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
#endif /* SUPPORT_PROFILING */

    spaceService->unlock();
}

void BGE::LogicService::addGameObject(GameObject *gameObject) {
    if (!gameObject) {
        return;
    }

    addRemoveLock();
    
    auto spaceHandle = gameObject->getSpace()->getHandle();
    auto gameObjHandle = gameObject->getHandle();
    
    LogicElement element{Operation::Add, spaceHandle, gameObjHandle};
    addRemoveQueue_.push_back(element);
    
    addRemoveUnlock();
}

void BGE::LogicService::removeGameObject(GameObject *gameObject) {
    addRemoveLock();
    
    auto spaceHandle = gameObject->getSpace()->getHandle();
    auto gameObjHandle = gameObject->getHandle();

    LogicElement element{Operation::Remove, spaceHandle, gameObjHandle};
    addRemoveQueue_.push_back(element);

    addRemoveUnlock();
}

void BGE::LogicService::spaceReset(Space *space) {
    executeLock();
    addRemoveLock();

    auto spaceHandle = space->getHandle();
    
    for (auto it=gameObjectHandles_.begin();it!=gameObjectHandles_.end();) {
        if (it->first == spaceHandle) {
#if DEBUG
            auto gameObj = space->getGameObject(it->second);
            if (gameObj) {
                printf("WARNING: removing logic handler for space %s, gameObj %s\n", space->getName().c_str(), gameObj->getName().c_str());
            } else {
                printf("WARNING: removing logic handler for space %s, gameObj NULL\n", space->getName().c_str());
            }
#endif
            it = gameObjectHandles_.erase(it);
        } else {
            it++;
        }
    }

    
    for (auto it=addRemoveQueue_.begin();it!=addRemoveQueue_.end();) {
        if (it->spaceHandle == spaceHandle) {
#if DEBUG
            auto gameObj = space->getGameObject(it->gameObjHandle);
            if (gameObj) {
                printf("WARNING: removing deferred logic handler %d for space %s, gameObj NULL\n", it->op, space->getName().c_str());
            }
#endif
            it = addRemoveQueue_.erase(it);
        } else {
            it++;
        }
    }
    
    addRemoveUnlock();
    executeUnlock();
}

void BGE::LogicService::addRemoveLock() {
    if (pthread_mutex_lock(&addRemoveMutex_)) {
        assert(false);
    }
}

void BGE::LogicService::addRemoveUnlock() {
    if (pthread_mutex_unlock(&addRemoveMutex_)) {
        assert(false);
    }
}

void BGE::LogicService::executeLock() {
    if (pthread_mutex_lock(&executeMutex_)) {
        assert(false);
    }
}

void BGE::LogicService::executeUnlock() {
    if (pthread_mutex_unlock(&executeMutex_)) {
        assert(false);
    }
}
