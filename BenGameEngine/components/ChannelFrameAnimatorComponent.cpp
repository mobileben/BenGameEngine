//
//  ChannelFrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ChannelFrameAnimatorComponent.h"

std::shared_ptr<BGE::ChannelFrameAnimatorComponent> BGE::ChannelFrameAnimatorComponent::create(ObjectId componentId) {
    return std::make_shared<ChannelFrameAnimatorComponent>(private_key{}, componentId);
}

BGE::ChannelFrameAnimatorComponent::ChannelFrameAnimatorComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), currKeyframe(0) {
}
