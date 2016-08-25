//
//  RenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderComponent.h"
#include "Game.h"

BGE::RenderComponent::RenderComponent() : Component(), localBounds_({0, 0, 0, 0}), globalBounds_({0, 0, 0, 0}), enabled_(true), globalBoundsDirty_(true), anchor_(RenderComponentAnchor::Center) {
}

void BGE::RenderComponent::destroy() {
    enabled_ = false;
    
    auto materialService = Game::getInstance()->getMaterialService();
    
    for (auto const &material : materialHandles_) {
        materialService->removeMaterial(material);
    }
    
    materialHandles_.clear();
    
    // Component::destroy last
    Component::destroy();
}

void BGE::RenderComponent::getGlobalBounds(Rect& bounds) {
    
}

BGE::MaterialHandle BGE::RenderComponent::getMaterialHandle(uint32_t index) {
    if (index < materialHandles_.size()) {
        return materialHandles_[index];
    } else {
        return MaterialHandle();
    }
}

BGE::Material *BGE::RenderComponent::getMaterial(uint32_t index) {
    if (index < materialHandles_.size()) {
        auto handle = materialHandles_[index];
        return Game::getInstance()->getMaterialService()->getMaterial(handle);
    }
    
    return nullptr;
}

void BGE::RenderComponent::setMaterials(std::vector<MaterialHandle> materials) {
    for (auto const &handle : materialHandles_) {
        if (std::find(materials.begin(), materials.end(), handle) == materials.end()) {
            // Make sure handle is not in materials
            Game::getInstance()->getMaterialService()->removeMaterial(handle);
        }
    }

    materialHandles_.clear();
    
    for (auto const &material : materials) {
        materialHandles_.push_back(material);
    }
    
    materialsUpdated();
}
