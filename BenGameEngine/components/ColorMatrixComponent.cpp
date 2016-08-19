//
//  ColorMatrixComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorMatrixComponent.h"

uint32_t BGE::ColorMatrixComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::ColorMatrixComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::ColorMatrixComponent::type_index_ = typeid(BGE::ColorMatrixComponent);

std::shared_ptr<BGE::ColorMatrixComponent> BGE::ColorMatrixComponent::create(ObjectId componentId) {
    return std::make_shared<ColorMatrixComponent>(private_key{}, componentId);
}

BGE::ColorMatrixComponent::ColorMatrixComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
    ColorMatrixMakeIdentify(matrix);
}