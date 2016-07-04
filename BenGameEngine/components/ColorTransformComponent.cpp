//
//  ColorTransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorTransformComponent.h"

uint32_t BGE::ColorTransformComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::ColorTransformComponent::type_index_ = typeid(BGE::ColorTransformComponent);

std::shared_ptr<BGE::ColorTransformComponent> BGE::ColorTransformComponent::create(ObjectId componentId) {
    return std::make_shared<ColorTransformComponent>(private_key{}, componentId);
}

BGE::ColorTransformComponent::ColorTransformComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
    ColorTransformMakeIdentity(transform);
}