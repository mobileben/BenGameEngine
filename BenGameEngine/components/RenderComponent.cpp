//
//  RenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderComponent.h"

BGE::RenderComponent::RenderComponent(uint64_t componentId) : Component(componentId), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(RenderComponentAnchor::Center) {
    
}

BGE::RenderComponent::RenderComponent(uint64_t componentId, std::string name) : Component(componentId, name), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(RenderComponentAnchor::Center) {
    
}

BGE::RenderComponent::RenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject) : Component(componentId, name, gameObject), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(RenderComponentAnchor::Center) {
    
}

void BGE::RenderComponent::getGlobalBounds(BGERect& bounds) {
    
}

std::weak_ptr<BGE::Material> BGE::RenderComponent::getMaterial(uint32_t index) {
    if (index < materials_.size()) {
        return materials_[index];
    } else {
        return std::weak_ptr<BGE::Material>();
    }
}

void BGE::RenderComponent::setMaterials(std::vector<std::shared_ptr<BGE::Material>> materials) {
    materials_.clear();
    
    for (auto material : materials) {
        materials_.push_back(material);
    }
    
    materialsUpdated();
}
