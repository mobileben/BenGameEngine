//
//  ChannelFrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ChannelFrameAnimatorComponent.h"

std::shared_ptr<BGE::ChannelFrameAnimatorComponent> BGE::ChannelFrameAnimatorComponent::create(uint64_t componentId) {
    return std::make_shared<ChannelFrameAnimatorComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::ChannelFrameAnimatorComponent> BGE::ChannelFrameAnimatorComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<ChannelFrameAnimatorComponent>(private_key{}, componentId, name);
}

BGE::ChannelFrameAnimatorComponent::ChannelFrameAnimatorComponent(struct private_key const& key, uint64_t componentId) : Component(componentId), currKeyframe(0) {
}

BGE::ChannelFrameAnimatorComponent::ChannelFrameAnimatorComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name), currKeyframe(0) {
}
