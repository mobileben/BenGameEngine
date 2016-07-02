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

BGE::AnimationService::AnimationService() {
}

BGE::AnimationService::~AnimationService() {
}

void BGE::AnimationService::update(double deltaTime) {
    float dt = (float)deltaTime;

    // TODO: Remove this since all game objects are in spaces
    for (auto obj : Game::getInstance()->getGameObjectService()->getGameObjects()) {
        auto animSeq = obj.second->getComponent<AnimationSequenceComponent>();
        auto animator = obj.second->getComponent<AnimatorComponent>();
        
        if (animSeq && animator) {
            animateSequence(animSeq, animator, dt);
        }
    }
    
    // For all the spaces
    for (auto handle : Game::getInstance()->getSpaceService()->getSpaces()) {
        auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
        
        if (space) {
            for (auto obj : space->getGameObjectService()->getGameObjects()) {
                auto animSeq = obj.second->getComponent<AnimationSequenceComponent>();
                auto animator = obj.second->getComponent<AnimatorComponent>();
                
                if (animSeq && animator) {
                    animateSequence(animSeq, animator, dt);
                }
            }
        }
    }
}

void BGE::AnimationService::animateSequence(std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<AnimatorComponent> animator, float deltaTime) {
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
            std::shared_ptr<Material> material;
            
            if (obj->getComponent<LineRenderComponent>()) {
                std::shared_ptr<LineRenderComponent> render = obj->getComponent<LineRenderComponent>();
                material = render->getMaterial().lock();
            } else if (obj->getComponent<FlatRectRenderComponent>()) {
                std::shared_ptr<FlatRectRenderComponent> render = obj->getComponent<FlatRectRenderComponent>();
                material = render->getMaterial().lock();
            } else if (obj->getComponent<SpriteRenderComponent>()) {
                std::shared_ptr<SpriteRenderComponent> render = obj->getComponent<SpriteRenderComponent>();
                material = render->getMaterial().lock();
            } else if (obj->getComponent<TextComponent>()) {
                std::shared_ptr<TextComponent> render = obj->getComponent<TextComponent>();
                material = render->getMaterial().lock();
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
            
            auto colorMatrix = obj->getComponent<ColorMatrixComponent>();
            auto colorTransform = obj->getComponent<ColorTransformComponent>();
            auto space = obj->getSpace();
            
            if (keyframe->colorMatrix) {
                if (colorMatrix) {
                    colorMatrix->matrix = *keyframe->colorMatrix;
                } else {
                    colorMatrix = space->createComponent<ColorMatrixComponent>();
                    obj->addComponent(colorMatrix);
                }
                
                colorMatrix->matrix = *keyframe->colorMatrix;

                if (material) {
                    material->setColorMatrix(*keyframe->colorMatrix);
                    NSLog(@"Color is %f %f %f %f", keyframe->colorMatrix->offset.r, keyframe->colorMatrix->offset.g, keyframe->colorMatrix->offset.b, keyframe->colorMatrix->offset.a);
                }
            } else {
                if (colorMatrix) {
                    obj->removeComponent<ColorMatrixComponent>();
                }
            }
                
            
            if (keyframe->colorTransform) {
                if (colorTransform) {
                    colorTransform->transform = *keyframe->colorTransform;
                } else {
                    colorTransform = space->createComponent<ColorTransformComponent>();
                    obj->addComponent(colorTransform);
                }
                
                colorTransform->transform = *keyframe->colorTransform;
            } else {
                if (colorTransform) {
                    obj->removeComponent<ColorTransformComponent>();
                }
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
                                animateSequenceByFrame(childSeq, childAnimator, keyframe->frame);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void BGE::AnimationService::animateSequenceByFrame(std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<FrameAnimatorComponent> animator, int32_t frame) {
    if (true || frame != animator->currentFrame) {
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


