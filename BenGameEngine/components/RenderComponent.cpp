//
//  RenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderComponent.h"

BGE::RenderComponent::RenderComponent(ObjectId componentId) : Component(componentId), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(RenderComponentAnchor::Center) {
    
}

void BGE::RenderComponent::getGlobalBounds(Rect& bounds) {
    
}

std::weak_ptr<BGE::Material> BGE::RenderComponent::getMaterial(uint32_t index) {
    if (index < materials_.size()) {
        return materials_[index];
    } else {
        return std::weak_ptr<BGE::Material>();
    }
}

void BGE::RenderComponent::setMaterials(std::vector<std::shared_ptr<Material>> materials) {
    materials_.clear();
    
    for (auto material : materials) {
        materials_.push_back(material);
    }
    
    materialsUpdated();
}
