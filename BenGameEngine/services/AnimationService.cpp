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

void BGE::AnimationService::update(double deltaTime) {
    lock();
    
    float dt = (float)deltaTime;

    // For all the spaces
    for (auto handle : Game::getInstance()->getSpaceService()->getSpaces()) {
        auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
        
        if (space && space->isVisible()) {
            for (auto const &handle : space->getGameObjects()) {
                auto obj = space->getGameObject(handle);
                
                if (obj) {
                    auto animSeq = obj->getComponent<AnimationSequenceComponent>();
                    auto animator = obj->getComponent<AnimatorComponent>();
                    
                    if (animSeq && animator) {
                        animateSequence(space, animSeq, animator, dt);
                    }
                }
            }
        }
    }
    
    processEvents();
    
    // TODO: Does unlocking happen before processing events?
    unlock();
}

BGE::EventHandlerHandle BGE::AnimationService::registerEventHandler(GameObject *gameObj, Event event, EventHandlerFunction function) {
    auto handle = eventService_->createEventHandlerHandle(gameObj, event, function);
    std::vector<EventHandlerHandle> &v = eventHandlers_[event];
    
    v.push_back(handle);
    
    return handle;
}

void BGE::AnimationService::unregisterEventHandler(EventHandlerHandle handle) {
    if (handle.isNull()) {
        return;
    }
    
    // Remove event handle from vector
    for (auto &mapIt : eventHandlers_) {
        auto it = std::find(mapIt.second.begin(), mapIt.second.end(), handle);
        
        if (it != mapIt.second.end()) {
            mapIt.second.erase(it);
            eventService_->removeEventHandler(handle);
            break;
        }
    }
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
                printf("WARNING: removing anim handler for space %s, gameObj %s\n", space->getName().c_str(), gameObj->getName().c_str());
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
    
    for (auto const &event : events_) {
        auto space = Game::getInstance()->getSpaceService()->getSpace(event.spaceHandle);
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
            if (adjustedDeltaTime >= animator->frameRemainderTime) {
                // Trim off the time left in the frame
                adjustedDeltaTime -= animator->frameRemainderTime;

                // Update our frame
                if (animator->forward) {
                    frame++;
                    
                    if (frame >= seq->totalFrames) {
                        // We're done!
                        frame = handleEndOfAnim(animator, 0, seq->totalFrames - 1);
                        triggerEvent = true;
                    }
                } else {
                    frame--;
                    
                    if (frame < 0) {
                        // We're done
                        frame = handleEndOfAnim(animator, seq->totalFrames - 1, 0);
                        triggerEvent = true;
                    }
                }

                // Make sure we're still playing
                if (animator->state == AnimState::Playing) {
                    // Resolve our time by stepping over any frames we missed
                    while (animator->state == AnimState::Playing && adjustedDeltaTime >= animator->secPerFrame) {
                        if (animator->forward) {
                            frame++;
                            
                            if (frame >= seq->totalFrames) {
                                // We're done!
                                frame = handleEndOfAnim(animator, 0, seq->totalFrames - 1);
                                triggerEvent = true;
                            }
                        } else {
                            frame--;
                            
                            if (frame < 0) {
                                // We're done
                                frame = handleEndOfAnim(animator, seq->totalFrames - 1, 0);
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
                // We need to update frames
                animator->currentFrame = frame;
                
                for (auto const &childHandle : seq->channels) {
                    auto childObj = space->getGameObject(childHandle);
                    
                    animateChannel(childObj, frame);
                }
            }
            
            if (triggerEvent) {
                if (animator->forward) {
                    queueEvent(space->getHandle(), animator->getGameObject()->getHandle(), Event::AnimationReachedEnd);
                } else {
                    queueEvent(space->getHandle(), animator->getGameObject()->getHandle(), Event::AnimationReachedBeginning);
                }
            }
        }
   }
}

void BGE::AnimationService::animateChannel(GameObject *gameObj, int32_t frame) {
    auto xform = gameObj->getComponent<TransformComponent>();
    auto channel = gameObj->getComponent<AnimationChannelComponent>();
    auto animator = gameObj->getComponent<ChannelFrameAnimatorComponent>();
    int32_t origFrame = animator->currKeyframe;
    int32_t channelFrame = origFrame;
    AnimationKeyframeReference *keyframe = channel->channel->keyframes[channelFrame];
    bool hide = false;
    
    if (frame < keyframe->startFrame) {
        // We need to start stepping back keyframes
        while (--channelFrame >= 0) {
            keyframe = channel->channel->keyframes[channelFrame];
            
            if (frame >= keyframe->startFrame) {
                break;
            }
        }
        
        if (channelFrame < 0) {
            channelFrame = 0;
            keyframe = channel->channel->keyframes[channelFrame];
            hide = true;
        }
    } else if (frame > (keyframe->startFrame + keyframe->totalFrames) ) {
        // We need to start stepping forward keyframes
        while (++channelFrame < channel->channel->numKeyframes) {
            keyframe = channel->channel->keyframes[channelFrame];
            
            if (frame < (keyframe->startFrame + keyframe->totalFrames)) {
                break;
            }
        }
        
        if (channelFrame >= channel->channel->numKeyframes) {
            channelFrame = channel->channel->numKeyframes - 1;
            keyframe = channel->channel->keyframes[channelFrame];
            hide = true;
        }
    } else {
        // We have the correct keyframe
    }
    
    if (hide) {
        xform->setVisibility(false);
    } else {
        xform->setVisibility(true);
        
        if (true || origFrame != channelFrame) {
            Material *material = nullptr;
            
            if (gameObj->getComponent<LineRenderComponent>()) {
                auto render = gameObj->getComponent<LineRenderComponent>();
                material = render->getMaterial();
            } else if (gameObj->getComponent<FlatRectRenderComponent>()) {
                auto render = gameObj->getComponent<FlatRectRenderComponent>();
                material = render->getMaterial();
            } else if (gameObj->getComponent<SpriteRenderComponent>()) {
                auto render = gameObj->getComponent<SpriteRenderComponent>();
                material = render->getMaterial();
            } else if (gameObj->getComponent<TextComponent>()) {
                auto render = gameObj->getComponent<TextComponent>();
                material = render->getMaterial();
            }
            
            animator->currKeyframe = channelFrame;
            
            // Update our transform
            if (keyframe->position) {
                xform->setPosition(*keyframe->position);
            } else {
                xform->setX(0);
                xform->setY(0);
            }
            if (keyframe->scale) {
                xform->setScale(*keyframe->scale);
            } else {
                xform->setScaleX(1);
                xform->setScaleY(1);
            }
            
            auto colorMatrix = gameObj->getComponent<ColorMatrixComponent>();
            auto colorTransform = gameObj->getComponent<ColorTransformComponent>();
            auto space = gameObj->getSpace();
            
            if (keyframe->colorMatrix) {
                if (colorMatrix) {
                    colorMatrix->matrix = *keyframe->colorMatrix;
                } else {
                    colorMatrix = space->createComponent<ColorMatrixComponent>();
                    gameObj->addComponent(colorMatrix);
                }
                
                colorMatrix->matrix = *keyframe->colorMatrix;

                if (material) {
                    material->setColorMatrix(*keyframe->colorMatrix);
                    NSLog(@"Color is %f %f %f %f", keyframe->colorMatrix->offset.r, keyframe->colorMatrix->offset.g, keyframe->colorMatrix->offset.b, keyframe->colorMatrix->offset.a);
                }
            } else {
                if (colorMatrix) {
                    gameObj->removeComponent<ColorMatrixComponent>();
                }
            }
                
            
            if (keyframe->colorTransform) {
                if (colorTransform) {
                    colorTransform->transform = *keyframe->colorTransform;
                } else {
                    colorTransform = space->createComponent<ColorTransformComponent>();
                    gameObj->addComponent(colorTransform);
                }
                
                colorTransform->transform = *keyframe->colorTransform;
            } else {
                if (colorTransform) {
                    gameObj->removeComponent<ColorTransformComponent>();
                }
            }
            
            // Update our render
            //DOTHESTUFF HERE
            
            xform->setRotationInDegrees(keyframe->rotation);
            
            // If this is a Keyframe reference, update
            if (channel->channel->referenceType == GfxReferenceTypeKeyframe) {
                // Find the appropriate child
                for (auto i=0;i<xform->getNumChildren();i++) {
                    auto childXform = xform->childAtIndex(i);
                    if (childXform->hasGameObject()) {
                        auto childObjHandle = childXform->getGameObjectHandle();
                        auto childObj = childXform->getSpace()->getGameObject(childObjHandle);
                        
                        // TODO: Have some better means of identifying the right child. For now brute force it
                        if (childObj) {
                            auto childAnimator = childObj->getComponent<FrameAnimatorComponent>();
                            
                            if (childAnimator) {
                                auto childSeq = childObj->getComponent<AnimationSequenceComponent>();
                                animateSequenceByFrame(space, childSeq, childAnimator, keyframe->frame);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void BGE::AnimationService::animateSequenceByFrame(Space *space, AnimationSequenceComponent *seq, FrameAnimatorComponent *animator, int32_t frame) {
    // TODO: UPDATE THIS
    if (true || frame != animator->currentFrame) {
        animator->currentFrame = frame;
        
        for (auto const &childHandle : seq->channels) {
            auto child = space->getGameObject(childHandle);
            animateChannel(child, frame);
        }
    }
}

int32_t BGE::AnimationService::handleEndOfAnim(AnimatorComponent *animator, int32_t startFrame, int32_t lastFrame) {
    int32_t frame = lastFrame;
    bool reset = false;
    
    switch (animator->iterations) {
        case AnimatorComponent::AnimPlayForever:
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
            break;
    }
    
    if (reset) {
        frame = startFrame;
    }
    
    return frame;
}


