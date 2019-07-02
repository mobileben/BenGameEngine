//
//  GameObjectService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GameObjectService.h"
#include "GameObject.h"
#include "Game.h"
#include "TransformComponent.h"

BGE::GameObjectService::GameObjectHandleService BGE::GameObjectService::handleService_(InitialGameObjectReserve, HandleServiceNoMaxLimit);

BGE::GameObjectService::GameObjectService() : Service() {
}

uint32_t BGE::GameObjectService::numGameObjects () const {
    auto pointers = handleService_.activePointers();
    
    return (uint32_t) pointers.size();
}

uint32_t BGE::GameObjectService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::GameObjectService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::GameObjectService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::GameObjectService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

size_t BGE::GameObjectService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::GameObjectService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::GameObjectService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

BGE::GameObject *BGE::GameObjectService::createGameObject(std::string name) {
    GameObjectHandle handle;
    auto obj = handleService_.allocate(handle);
    
    if (obj) {
        obj->initialize(spaceHandle_, handle, name);
        gameObjects_.push_back(handle);
    }
    
    return obj;
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(ObjectId objId) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObject(handle);
        
        if (obj) {
            if (obj->getInstanceId() == objId) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandleLockless(ObjectId objId) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObjectLockless(handle);
        
        if (obj) {
            if (obj->getInstanceId() == objId) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(std::string name) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObject(handle);
        
        if (obj) {
            if (obj->getName() == name) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandleLockless(std::string name) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObjectLockless(handle);
        
        if (obj) {
            if (obj->getName() == name) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

void BGE::GameObjectService::getAllChildGameObjects(GameObject *root, std::vector<GameObject *> &objects) {
    if (root) {
        auto xform = root->getComponent<TransformComponent>();
        
        if (xform) {
            auto children = xform->getChildren();
            
            for (auto &child : children) {
                auto object = child->getGameObject();
                
                if (object) {
                    objects.push_back(object);
                }
                
                getAllChildGameObjects(object, objects);
            }
        }
    } else {
        objects.clear();
    }
}

void BGE::GameObjectService::removeGameObject(GameObject *object) {
    if (object) {
        object->setActive(false);
        
        auto xform = object->getComponent<TransformComponent>();
        
        if (xform) {
            xform->removeFromParent();
        }

        lock();
        childGameObjectsScratch_.clear();
        getAllChildGameObjects(object, childGameObjectsScratch_);

        // Add the game object to the end to ensure it gets removed
        childGameObjectsScratch_.push_back(object);
        for (auto object : childGameObjectsScratch_) {
            auto handle = object->getHandle();
            
            for (auto it = gameObjects_.begin();it != gameObjects_.end();++it) {
                if (*it == handle) {
                    releaseObject(handle);
                    gameObjects_.erase(it);
                    break;
                }
            }
        }
        unlock();
    }
}

void BGE::GameObjectService::removeAllGameObjects() {
    for (auto const &handle : gameObjects_) {
        releaseObjectDontReleaseComponents(handle);
    }
    
    gameObjects_.clear();
}


void BGE::GameObjectService::releaseObject(GameObjectHandle handle) {
    auto obj = getGameObject(handle);

    if (obj) {
        obj->destroy();
        handleService_.release(handle);
    }
}

void BGE::GameObjectService::releaseObjectDontReleaseComponents(GameObjectHandle handle){
    auto obj = getGameObject(handle);

    if (obj) {
        obj->destroyDontReleaseComponents();
        handleService_.release(handle);
    }
}

BGE::Space *BGE::GameObjectService::getSpace(void) const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}

