//
//  ColorMatrixComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorMatrixComponent.h"

uint32_t BGE::ColorMatrixComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::ColorMatrixComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::ColorMatrixComponent::type_index_ = typeid(BGE::ColorMatrixComponent);

BGE::ColorMatrixComponent::ColorMatrixComponent() : Component() {
    ColorMatrixMakeIdentify(matrix);
}