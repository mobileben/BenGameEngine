//
//  ColorTransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorTransformComponent.h"

std::shared_ptr<BGE::ColorTransformComponent> BGE::ColorTransformComponent::create(uint64_t componentId) {
    return std::make_shared<ColorTransformComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::ColorTransformComponent> BGE::ColorTransformComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<ColorTransformComponent>(private_key{}, componentId, name);
}

BGE::ColorTransformComponent::ColorTransformComponent(struct private_key const& key, uint64_t componentId) : Component(componentId) {
}

BGE::ColorTransformComponent::ColorTransformComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name) {
}