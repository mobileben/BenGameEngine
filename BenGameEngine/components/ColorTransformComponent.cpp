//
//  ColorTransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ColorTransformComponent.h"

std::shared_ptr<BGE::ColorTransformComponent> BGE::ColorTransformComponent::create(ObjectId componentId) {
    return std::make_shared<ColorTransformComponent>(private_key{}, componentId);
}

BGE::ColorTransformComponent::ColorTransformComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
}