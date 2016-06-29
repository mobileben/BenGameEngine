//
//  FrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FrameAnimatorComponent.h"

std::shared_ptr<BGE::FrameAnimatorComponent> BGE::FrameAnimatorComponent::create(uint64_t componentId) {
    return std::make_shared<FrameAnimatorComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::FrameAnimatorComponent> BGE::FrameAnimatorComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<FrameAnimatorComponent>(private_key{}, componentId, name);
}

BGE::FrameAnimatorComponent::FrameAnimatorComponent(struct private_key const& key, uint64_t componentId) : Component(componentId), currentFrame(0) {
}

BGE::FrameAnimatorComponent::FrameAnimatorComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name), currentFrame(0) {
}
