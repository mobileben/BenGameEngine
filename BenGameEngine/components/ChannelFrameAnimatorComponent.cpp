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
