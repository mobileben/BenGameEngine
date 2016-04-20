//
//  MaterialService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MaterialService.h"

BGE::MaterialService::MaterialService() {
}

BGE::MaterialService::~MaterialService() {
}

void BGE::MaterialService::initialize() {}
void BGE::MaterialService::reset() {}
void BGE::MaterialService::enteringBackground() {}
void BGE::MaterialService::enteringForeground() {}
void BGE::MaterialService::pause() {}
void BGE::MaterialService::resume() {}
void BGE::MaterialService::destroy() {}

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(std::string name, BGEVector4& color) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<Material> material(new Material(objId, name, color));
    
    addMaterial(material);
    
    return material;
}

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(std::string name, std::shared_ptr<TextureBase> texture) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<Material> material(new Material(objId, name, texture));
    
    addMaterial(material);
    
    return material;
}

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(std::string name, BGEVector4& color, std::shared_ptr<TextureBase> texture) {
    uint64_t objId = getIdAndIncrement();
    std::shared_ptr<Material> material(new Material(objId, name, color));
    
    addMaterial(material);
    
    return material;
}

void BGE::MaterialService::addMaterial(std::shared_ptr<Material> material) {
    MaterialMapIterator it = materials_.find(material->getName());
    
    if (it != materials_.end()) {
        // We already have the material
        // TODO: Error, warning, override?
    } else {
        materials_[material->getName()] = material;
    }

}

void BGE::MaterialService::removeMaterial(std::string name) {
    materials_.erase(name);
}

std::shared_ptr<BGE::Material> BGE::MaterialService::materialWithName(std::string name) {
    if (materials_.find(name) != materials_.end()) {
        return materials_[name];
    } else {
        return nullptr;
    }
}
