//
//  AnimationService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimationService.h"
#include "Game.h"
#include "RenderComponent.h"
#include "AnimationSequenceComponent.h"
#include "AnimationChannelComponent.h"
#include "AnimatorComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "TransformComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "LineRenderComponent.h"
#ifdef SUPPORT_PROFILING
#include "Profiling.h"
#endif /* SUPPORT_PROFILING */

void BGE::AnimationService::update(double deltaTime) {
    auto spaceService = Game::getInstance()->getSpaceService();
    
    lock();
    spaceService->lock();

#ifdef SUPPORT_PROFILING
    auto startTime = profiling::EpochTime::timeInMicroSec();
    numProcessedObjects_ = 0;
#endif /* SUPPORT_PROFILING */

    float dt = (float)deltaTime;
    
    // For all the spaces
    spaceService->getSpaces(spaceHandles_);

    for (auto handle : spaceHandles_) {
        auto space = spaceService->getSpace(handle);
        
        if (space && space->isVisible()) {
            auto& objects = space->getAnimObjects();
            for (auto const &handle : objects) {
                handleServicesLock();
                auto obj = space->getGameObjectLockless(handle);
                
                if (obj && obj->isActive() && obj->isVisible()) {
                    auto animSeq = obj->getComponentLockless<AnimationSequenceComponent>(space);
                    auto animator = obj->getComponentLockless<AnimatorComponent>(space);
                    
                    if (animSeq) {
                        animateSequence(space, animSeq, animator, dt);
#ifdef SUPPORT_PROFILING
                        ++numProcessedObjects_;
#endif /* SUPPORT_PROFILING */
                    }
                }
                handleServicesUnlock();
            }
        }
    }
    
    spaceService->unlock();
    
    processEvents();

#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
#endif /* SUPPORT_PROFILING */

    // TODO: Does unlocking happen before processing events?
    unlock();
}

void BGE::AnimationService::garbageCollect() {
    eventService_->garbageCollect();
}

BGE::EventHandlerHandle BGE::AnimationService::registerEventHandler(GameObject *gameObj, Event event, EventHandlerFunction function) {
    lock();
    
    auto handle = eventService_->createEventHandlerHandle(gameObj, event, function);
    std::vector<EventHandlerHandle> &v = eventHandlers_[event];
    
    v.push_back(handle);
    
    unlock();
    
    return handle;
}

void BGE::AnimationService::unregisterEventHandler(EventHandlerHandle handle) {
    if (handle.isNull()) {
        return;
    }
    
    lock();
    
    // Remove event handle from vector
    for (auto &mapIt : eventHandlers_) {
        auto it = std::find(mapIt.second.begin(), mapIt.second.end(), handle);
        
        if (it != mapIt.second.end()) {
            mapIt.second.erase(it);
            eventService_->removeEventHandler(handle);
            break;
        }
    }
    
    unlock();
}

void BGE::AnimationService::spaceReset(Space *space) {
    lock();
    
    auto eventService = eventService_;
    auto spaceHandle = space->getHandle();
    
    for (auto &mapIt : eventHandlers_) {
        for (auto hIt=mapIt.second.begin();hIt!=mapIt.second.end();) {
            auto handle = *hIt;
            auto handler = eventService->getEventHandler(handle);
            
            if (handler->spaceHandle == spaceHandle) {
#if DEBUG
                auto gameObj = space->getGameObject(handler->gameObjHandle);
                
                if (gameObj) {
                    printf("WARNING: removing anim handler for space %s, gameObj %s\n", space->getName().c_str(), gameObj->getName().c_str());
                } else {
                    printf("WARNING: removing anim handler for space %s, gameObj nullptr\n", space->getName().c_str());
                }
#endif
                hIt = mapIt.second.erase(hIt);
                eventService->removeEventHandler(handle);
            } else {
                hIt++;
            }
        }
    }
    
    unlock();
}

void BGE::AnimationService::queueEvent(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, Event event) {
    // TODO: Mutex this
    AnimationEvent animEvent{ spaceHandle, gameObjHandle, event };
    
    events_.push_back(animEvent);
}

void BGE::AnimationService::processEvents() {
    auto eventService = eventService_;
    auto spaceService = Game::getInstance()->getSpaceService();
    
    for (auto const &event : events_) {
        auto space = spaceService->getSpace(event.spaceHandle);
        auto gameObj = space->getGameObject(event.gameObjHandle);
        
        if (gameObj) {
            // Now if we match an event handler, dispatch that too
            auto it = eventHandlers_.find(event.event);
            
            if (it != eventHandlers_.end()) {
                auto const &handles = it->second;
                auto gameObjHandle = gameObj->getHandle();
                
                for (auto const &handle : handles) {
                    auto handler = eventService->getEventHandler(handle);
                    
                    if (handler) {
                        if (gameObjHandle == handler->gameObjHandle) {
                            handler->handler(gameObj, event.event);
                        }
                    }
                }
            }
        } else {
            // buttonHandlers must be associated with active game objects
            assert(false);
        }
    }
    
    // TODO: Mutex out
    events_.clear();
}

void BGE::AnimationService::animateSequence(Space *space, AnimationSequenceComponent *seq, AnimatorComponent *animator, float deltaTime) {
    if (seq && animator) {
        if (animator->state == AnimState::Playing) {
            int32_t origFrame = animator->currentFrame;
            int32_t frame = origFrame;
            bool triggerEvent = false;
            float adjustedDeltaTime = animator->speed * deltaTime;
            
            // Are we going to step past our current frame?
            if (BGE::nearlyGreaterThanOrEqual(adjustedDeltaTime, animator->frameRemainderTime)) {
                // Trim off the time left in the frame
                adjustedDeltaTime -= animator->frameRemainderTime;

                // Update our frame
                if (animator->forward) {
                    frame++;
                    
                    if (frame > animator->endFrame) {
                        // We're done!
                        frame = handleEndOfAnim(animator, animator->startFrame, animator->endFrame);
                        triggerEvent = true;
                    }
                } else {
                    frame--;
                    
                    if (frame < animator->startFrame) {
                        // We're done
                        frame = handleEndOfAnim(animator, animator->endFrame, animator->startFrame);
                        triggerEvent = true;
                    }
                }

                // Make sure we're still playing
                if (animator->state == AnimState::Playing) {
                    // Resolve our time by stepping over any frames we missed
                    while (animator->state == AnimState::Playing && BGE::nearlyGreaterThanOrEqual(adjustedDeltaTime, animator->secPerFrame)) {
                        if (animator->forward) {
                            frame++;
                            
                            if (frame > animator->endFrame) {
                                // We're done!
                                frame = handleEndOfAnim(animator, animator->startFrame, animator->endFrame);
                                triggerEvent = true;
                            }
                        } else {
                            frame--;
                            
                            if (frame < animator->startFrame) {
                                // We're done
                                frame = handleEndOfAnim(animator, animator->endFrame, animator->startFrame);
                                triggerEvent = true;
                            }
                        }
                        
                        adjustedDeltaTime -= animator->secPerFrame;
                    }

                    if (adjustedDeltaTime < 0) {
                        adjustedDeltaTime = -adjustedDeltaTime;
                    }
                    
                    animator->frameRemainderTime = animator->secPerFrame - adjustedDeltaTime;
                } else {
                    // We've really finished
                    animator->frameRemainderTime = 0;
                }
            } else {
                animator->frameRemainderTime -= adjustedDeltaTime;
            }
            
            if (origFrame != frame) {
                animator->setFrame(space, frame);
            }
            
            // Only trigger when done
            if (triggerEvent && animator->state == AnimState::Done) {
                if (animator->forward) {
                    queueEvent(space->getHandle(), animator->getGameObject()->getHandle(), Event::AnimationReachedEnd);
                } else {
                    queueEvent(space->getHandle(), animator->getGameObject()->getHandle(), Event::AnimationReachedBeginning);
                }
            }
        }
   }
}

int32_t BGE::AnimationService::handleEndOfAnim(AnimatorComponent *animator, int32_t startFrame, int32_t lastFrame) {
    int32_t frame = lastFrame;
    bool reset = false;
    
    switch (animator->iterations) {
        case AnimPlayForever:
            reset = true;
            break;
            
        case 1:
            animator->iterations--;
        case 0:
            // We're done
            animator->state = AnimState::Done;
            break;
        default:
            animator->iterations--;
            reset = true;
            break;
    }
    
    if (reset) {
        frame = startFrame;
    }
    
    return frame;
}


