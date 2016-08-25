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
#include "BoundingBoxComponent.h"

uint32_t BGE::AnimatorComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::AnimatorComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimatorComponent::type_index_ = typeid(BGE::AnimatorComponent);

BGE::AnimatorComponent::AnimatorComponent() : Component(), state(AnimState::Done), currentFrame(0), iterations(1), frameRemainderTime(0), secPerFrame(1.0/30.0), speed(1), forward(true) {
}

void BGE::AnimatorComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    state = AnimState::Done;
    currentFrame = 0;
    iterations = 0;
    frameRemainderTime = 0;
    speed = 1;
    forward = true;
}

void BGE::AnimatorComponent::destroy() {
    state = AnimState::Done;
    currentFrame = 0;
    iterations = 0;
    frameRemainderTime = 0;
    speed = 1;
    forward = true;

    // Component::destroy last
    Component::destroy();
}

void BGE::AnimatorComponent::reset() {
    setFrame(0, true);
    iterations = 1;
    state = AnimState::Done;
    
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = getSpace()->getGameObject(gameObjHandle);
    auto seq = gameObj->getComponent<AnimationSequenceComponent>();
    
    if (seq->frameRate != 0) {
        secPerFrame = 1.0 / (float) seq->frameRate;
    } else {
        secPerFrame = 0;
    }
    
    frameRemainderTime = secPerFrame;
}

void BGE::AnimatorComponent::setFrame(int32_t frame, bool force) {
    auto space = getSpace();
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = space->getGameObject(gameObjHandle);
    
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
                auto chanGameObjHandle = seq->channels[i];
                auto chanGameObj = space->getGameObject(chanGameObjHandle);
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
            
            // Update the bounds
            for (auto i=0;i<seq->numBounds;i++) {
                auto bounds = seq->bounds[i];
                
                if (bounds.startFrame >= i && i < (bounds.startFrame + bounds.totalFrames)) {
                    auto bbox = gameObj->getComponent<BoundingBoxComponent>();
                    
                    if (bbox) {
                        bbox->x = bounds.bounds->x;
                        bbox->y = bounds.bounds->y;
                        bbox->width = bounds.bounds->w;
                        bbox->height = bounds.bounds->h;
                    }
                    break;
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
