//
//  ColorMatrixComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorMatrixComponent.h"

std::shared_ptr<BGE::ColorMatrixComponent> BGE::ColorMatrixComponent::create(uint64_t componentId) {
    return std::make_shared<ColorMatrixComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::ColorMatrixComponent> BGE::ColorMatrixComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<ColorMatrixComponent>(private_key{}, componentId, name);
}

BGE::ColorMatrixComponent::ColorMatrixComponent(struct private_key const& key, uint64_t componentId) : Component(componentId) {
    ColorMatrixMakeIdentify(matrix);
}

BGE::ColorMatrixComponent::ColorMatrixComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name) {
    ColorMatrixMakeIdentify(matrix);
}