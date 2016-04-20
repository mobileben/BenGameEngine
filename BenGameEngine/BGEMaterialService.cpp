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

std::shared_ptr<BGEMaterial> BGEMaterialService::createMaterial(std::string name, BGEVector4& color) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<BGEMaterial> material(new BGEMaterial(objId, name, color));
    
    addMaterial(material);
    
    return material;
}

std::shared_ptr<BGEMaterial> BGEMaterialService::createMaterial(std::string name, std::shared_ptr<BGETextureBase> texture) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<BGEMaterial> material(new BGEMaterial(objId, name, texture));
    
    addMaterial(material);
    
    return material;
}

std::shared_ptr<BGEMaterial> BGEMaterialService::createMaterial(std::string name, BGEVector4& color, std::shared_ptr<BGETextureBase> texture) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<BGEMaterial> material(new BGEMaterial(objId, name, color));
    
    addMaterial(material);
    
    return material;
}

void BGEMaterialService::addMaterial(std::shared_ptr<BGEMaterial> material) {
    MaterialMapIterator it = materials_.find(material->getName());
    
    if (it != materials_.end()) {
        // We already have the material
        // TODO: Error, warning, override?
    } else {
        materials_[material->getName()] = material;
    }

}

void BGEMaterialService::removeMaterial(std::string name) {
    materials_.erase(name);
}

std::shared_ptr<BGEMaterial> BGEMaterialService::materialWithName(std::string name) {
    if (materials_.find(name) != materials_.end()) {
        return materials_[name];
    } else {
        return nullptr;
    }
}
