//
//  FrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FrameAnimatorComponent.h"

uint32_t BGE::FrameAnimatorComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::FrameAnimatorComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::FrameAnimatorComponent::type_index_ = typeid(BGE::FrameAnimatorComponent);
