//
//  InputTouchComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "InputTouchComponent.h"

uint32_t BGE::InputTouchComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::InputTouchComponent::type_index_ = typeid(BGE::InputTouchComponent);

std::shared_ptr<BGE::InputTouchComponent> BGE::InputTouchComponent::create(ObjectId componentId) {
    return std::make_shared<InputTouchComponent>(private_key{}, componentId);
}

BGE::InputTouchComponent::InputTouchComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
}