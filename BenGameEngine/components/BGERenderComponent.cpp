//
//  BGERenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGERenderComponent.h"

BGERenderComponent::BGERenderComponent(uint64_t componentId) : BGEComponent(componentId), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(BGERenderComponentAnchor::Center) {
    
}

BGERenderComponent::BGERenderComponent(uint64_t componentId, std::string name) : BGEComponent(componentId, name), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(BGERenderComponentAnchor::Center) {
    
}

BGERenderComponent::BGERenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject) : BGEComponent(componentId, name, gameObject), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(BGERenderComponentAnchor::Center) {
    
}

void BGERenderComponent::getGlobalBounds(BGERect& bounds) {
    
}

std::weak_ptr<BGEMaterial> BGERenderComponent::getMaterial(uint32_t index) {
    if (index < materials_.size()) {
        return materials_[index];
    } else {
        return std::weak_ptr<BGEMaterial>();
    }
}

void BGERenderComponent::setMaterials(std::vector<std::shared_ptr<BGEMaterial>> materials) {
    materials_.clear();
    
    for (auto material : materials) {
        materials_.push_back(material);
    }
    
    materialsUpdated();
}
