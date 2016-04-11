//
//  BGEMaterialService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEMaterialService.h"

BGEMaterialService::BGEMaterialService() {
}

BGEMaterialService::~BGEMaterialService() {
}

void BGEMaterialService::initialize() {}
void BGEMaterialService::reset() {}
void BGEMaterialService::enteringBackground() {}
void BGEMaterialService::enteringForeground() {}
void BGEMaterialService::pause() {}
void BGEMaterialService::resume() {}
void BGEMaterialService::destroy() {}

void BGEMaterialService::addMaterial(std::string name, BGEVector4& color) {
}

void BGEMaterialService::addMaterial(std::string name, std::shared_ptr<BGETexture> texture) {
    
}

void BGEMaterialService::addMaterial(std::string name, BGEVector4& color, std::shared_ptr<BGETexture> texture) {
    
}

void BGEMaterialService::removeMaterial(std::string name) {
    
}

std::shared_ptr<BGEMaterial> BGEMaterialService::materialWithName(std::string name) {
    if (materials_.find(name) != materials_.end()) {
        return materials_[name];
    } else {
        return nullptr;
    }
}
