//
//  AnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimatorComponent.h"
#include "GameObject.h"
#include "AnimationSequenceComponent.h"
#include "TransformComponent.h"
#include "AnimationChannelComponent.h"
#include "ChannelFrameAnimatorComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "Space.h"

std::shared_ptr<BGE::AnimatorComponent> BGE::AnimatorComponent::create(ObjectId componentId) {
    return std::make_shared<AnimatorComponent>(private_key{}, componentId);
}

BGE::AnimatorComponent::AnimatorComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), state(AnimState::Done), currentFrame(0), iterations(1), frameRemainderTime(0), secPerFrame(1.0/30.0), speed(1), forward(true) {
}

void BGE::AnimatorComponent::reset() {
    setFrame(0, true);
    iterations = 1;
    state = AnimState::Done;
    
    auto gameObj = getGameObject().lock();
    auto seq = gameObj->getComponent<AnimationSequenceComponent>();
    
    if (seq->frameRate != 0) {
        secPerFrame = 1.0 / (float) seq->frameRate;
    } else {
        secPerFrame = 0;
    }
    
    frameRemainderTime = secPerFrame;
}

void BGE::AnimatorComponent::setFrame(int32_t frame, bool force) {
    auto gameObj = getGameObject().lock();
    
    if (gameObj) {
        auto seq = gameObj->getComponent<AnimationSequenceComponent>();
        
        if (seq) {
            if (currentFrame == frame && !force) {
                return;
            }
            
            currentFrame = frame;
            frameRemainderTime = secPerFrame;
            
            // Traverse our seq and setup all transforms, references, colors, etc
            for (auto i=0;i<seq->numChannels;i++) {
                auto chanGameObj = seq->channels[i];
                auto xform = chanGameObj->getComponent<TransformComponent>();
                auto channel = chanGameObj->getComponent<AnimationChannelComponent>();
                auto animator = chanGameObj->getComponent<ChannelFrameAnimatorComponent>();
                
                assert(chanGameObj);
                assert(xform);
                assert(channel);
                assert(animator);
                
                if (chanGameObj && xform && channel && animator) {
                    assert(channel->channel->keyframes);
                    
                    animator->currKeyframe = 0;
                    
                    if (channel->channel->keyframes) {
                        for (auto ki=0;ki<channel->channel->numKeyframes;ki++) {
                            auto keyframe = channel->channel->keyframes[ki];
                            
                            if (frame < keyframe->startFrame) {
                                break;
                            } else if (frame >= keyframe->startFrame && frame < (keyframe->startFrame + keyframe->totalFrames)) {
                                auto colorMatrix = chanGameObj->getComponent<ColorMatrixComponent>();
                                auto colorTransform = chanGameObj->getComponent<ColorTransformComponent>();
                                auto space = channel->getSpace();
                                animator->currKeyframe = ki;
                                xform->setPosition(*keyframe->position);
                                xform->setScale(*keyframe->scale);
                                xform->setRotation(keyframe->rotation);
                                
                                if (keyframe->colorMatrix) {
                                    if (colorMatrix) {
                                        colorMatrix->matrix = *keyframe->colorMatrix;
                                    } else {
                                        colorMatrix = space->createComponent<ColorMatrixComponent>();
                                        chanGameObj->addComponent(colorMatrix);
                                    }
                                    
                                    colorMatrix->matrix = *keyframe->colorMatrix;
                                } else {
                                    if (colorMatrix) {
                                        chanGameObj->removeComponent<ColorMatrixComponent>();
                                    }
                                }
                                
                                if (keyframe->colorTransform) {
                                    if (colorTransform) {
                                        colorTransform->transform = *keyframe->colorTransform;
                                    } else {
                                        colorTransform = space->createComponent<ColorTransformComponent>();
                                        chanGameObj->addComponent(colorTransform);
                                    }
                                    
                                    colorTransform->transform = *keyframe->colorTransform;
                                } else {
                                    if (colorTransform) {
                                        chanGameObj->removeComponent<ColorTransformComponent>();
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void BGE::AnimatorComponent::play(int32_t iterations, bool forward, float speed) {
    this->state = AnimState::Playing;
    this->iterations = iterations;
    this->forward = forward;
    this->speed = speed;
    
    this->setFrame(0);
}

void BGE::AnimatorComponent::pause() {
    this->state = AnimState::Paused;
}

void BGE::AnimatorComponent::stop() {
    // TODO: Do we send this event to listeners?
    this->state = AnimState::Done;
}
