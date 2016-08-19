//
//  ChannelFrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ChannelFrameAnimatorComponent.h"

uint32_t BGE::ChannelFrameAnimatorComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::ChannelFrameAnimatorComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::ChannelFrameAnimatorComponent::type_index_ = typeid(BGE::ChannelFrameAnimatorComponent);

std::shared_ptr<BGE::ChannelFrameAnimatorComponent> BGE::ChannelFrameAnimatorComponent::create(ObjectId componentId) {
    return std::make_shared<ChannelFrameAnimatorComponent>(private_key{}, componentId);
}

BGE::ChannelFrameAnimatorComponent::ChannelFrameAnimatorComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), currKeyframe(0) {
}
