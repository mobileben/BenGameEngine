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

BGE::AnimationService::AnimationService() {
}

BGE::AnimationService::~AnimationService() {
}

void BGE::AnimationService::update(double deltaTime) {
    float dt = (float)deltaTime;
    for (auto obj : Game::getInstance()->getGameObjectService()->getGameObjects()) {
        auto animSeq = obj.second->getComponent<AnimationSequenceComponent>();
        auto animator = obj.second->getComponent<AnimatorComponent>();
        
        if (animSeq && animator) {
            animateSequence(animSeq, animator, dt);
        }
    }
}

void BGE::AnimationService::animateSequence(std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<AnimatorComponent> animator, float deltaTime) {
    if (seq && animator) {
        if (animator->state == AnimState::Playing) {
            int32_t origFrame = animator->currentFrame;
            int32_t frame = origFrame;
            bool triggerEvent = false;
            
            // Are we going to step past our current frame?
            if (deltaTime >= animator->frameRemainderTime) {
                // Trim off the time left in the frame
                deltaTime -= animator->frameRemainderTime;

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
                    while (animator->state == AnimState::Playing && deltaTime >= animator->secPerFrame) {
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
                        
                        deltaTime -= animator->secPerFrame;
                    }

                    if (deltaTime < 0) {
                        deltaTime = -deltaTime;
                    }
                    
                    animator->frameRemainderTime = deltaTime;
                } else {
                    // We've really finished
                    animator->frameRemainderTime = 0;
                }
            } else {
                animator->frameRemainderTime -= deltaTime;
            }
            
            if (origFrame != frame) {
                // We need to update frames
                animator->currentFrame = frame;
                
                for (auto child : seq->channels) {
                    animateChannel(child, frame);
                }
            }
            
            if (triggerEvent) {
                
            }
        }
   }
}

void BGE::AnimationService::animateChannel(std::shared_ptr<GameObject> obj, int32_t frame) {
    auto xform = obj->getComponent<TransformComponent>();
    auto channel = obj->getComponent<AnimationChannelComponent>();
    auto animator = obj->getComponent<ChannelFrameAnimatorComponent>();
    int32_t origFrame = animator->currKeyframe;
    int32_t channelFrame = origFrame;
    AnimationKeyframeReference *keyframe = channel->channel->keyframes[channelFrame];
    
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
        }
    } else {
        // We have the correct keyframe
    }
    
    if (origFrame != channelFrame) {
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
            xform->setScaleY(0);
        }

        // Update our render
        //DOTHESTUFF HERE

        xform->setRotation(keyframe->rotation);
        
        // If this is a Keyframe reference, update
        if (channel->channel->referenceType == GfxReferenceTypeKeyframe) {
            // Find the appropriate child
            for (auto i=0;i<xform->getNumChildren();i++) {
                auto childXform = xform->childAtIndex(i);
                if (childXform->hasGameObject()) {
                    auto childObj = childXform->getGameObject().lock();
                    
                    // TODO: Have some better means of identifying the right child. For now brute force it
                    if (childObj) {
                        auto childAnimator = childObj->getComponent<FrameAnimatorComponent>();
                        
                        if (childAnimator) {
                            auto childSeq = childObj->getComponent<AnimationSequenceComponent>();
                            animateSequenceByFrame(childSeq, childAnimator, channel->channel->animation.frame);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void BGE::AnimationService::animateSequenceByFrame(std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<FrameAnimatorComponent> animator, int32_t frame) {
    if (frame != animator->currentFrame) {
        animator->currentFrame = frame;
        
        for (auto child : seq->channels) {
            animateChannel(child, frame);
        }
    }
}

int32_t BGE::AnimationService::handleEndOfAnim(std::shared_ptr<AnimatorComponent> animator, int32_t startFrame, int32_t lastFrame) {
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


