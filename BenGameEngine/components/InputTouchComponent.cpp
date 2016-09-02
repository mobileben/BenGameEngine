//
//  InputTouchComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "InputTouchComponent.h"

uint32_t BGE::InputTouchComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::InputTouchComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::InputTouchComponent::type_index_ = typeid(BGE::InputTouchComponent);
