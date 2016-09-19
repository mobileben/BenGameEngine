//
//  ColorTransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorTransformComponent.h"

uint32_t BGE::ColorTransformComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::ColorTransformComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::ColorTransformComponent::type_index_ = typeid(BGE::ColorTransformComponent);

BGE::ColorTransformComponent::ColorTransformComponent() : Component() {
    ColorTransformMakeIdentity(transform);
}