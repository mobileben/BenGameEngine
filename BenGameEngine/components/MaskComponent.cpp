//
//  MaskComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MaskComponent.h"

uint32_t BGE::MaskComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::MaskComponent::type_index_ = typeid(BGE::MaskComponent);

std::shared_ptr<BGE::MaskComponent> BGE::MaskComponent::create(ObjectId componentId) {
    return std::make_shared<MaskComponent>(private_key{}, componentId);
}

BGE::MaskComponent::MaskComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
}

void BGE::MaskComponent::setMaskReference(MaskReference *maskRef) {
    if (maskRef) {
        setMaskReference(*maskRef);
    }
}

void BGE::MaskComponent::setMaskReference(const MaskReference &maskRef) {
    
}
