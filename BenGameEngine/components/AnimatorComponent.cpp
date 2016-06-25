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

std::shared_ptr<BGE::AnimatorComponent> BGE::AnimatorComponent::create(uint64_t componentId) {
    return std::make_shared<AnimatorComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::AnimatorComponent> BGE::AnimatorComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<AnimatorComponent>(private_key{}, componentId, name);
}

BGE::AnimatorComponent::AnimatorComponent(struct private_key const& key, uint64_t componentId) : Component(componentId), state(AnimState::Done), currentFrame(0), iterations(1), frameRemainderTime(0), secPerFrame(1.0/30.0), speed(1), forward(true) {
}

BGE::AnimatorComponent::AnimatorComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name), state(AnimState::Done), currentFrame(0), iterations(1), frameRemainderTime(0), secPerFrame(1.0/30.0), speed(1), forward(true) {
}

void BGE::AnimatorComponent::reset() {
    setFrame(0, true);
    iterations = 1;
    frameRemainderTime = 0;
    state = AnimState::Done;
}

void BGE::AnimatorComponent::setFrame(int32_t frame, bool force) {
    auto gameObj = getGameObject().lock();
    
    if (gameObj) {
        auto seq = gameObj->getComponent<AnimationSequenceComponent>();
        
        if (seq) {
            if (currentFrame == frame && !force) {
                return;
            }
            
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
                    if (channel->channel->keyframes) {
                        for (auto ki=0;ki<channel->channel->numKeyframes;ki++) {
                            auto keyframe = channel->channel->keyframes[ki];
                            
                            if (frame < keyframe->startFrame) {
                                break;
                            } else if (frame >= keyframe->startFrame && frame < (keyframe->startFrame + keyframe->totalFrames)) {
                                xform->setPosition(*keyframe->position);
                                xform->setScale(*keyframe->scale);
                                xform->setRotation(keyframe->rotation);
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
    this->iterations = 1;
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
