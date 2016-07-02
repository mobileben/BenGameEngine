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

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(Vector4& color) {
    ObjectId objId = getIdAndIncrement();
    std::shared_ptr<Material> material = Material::create(objId);
    
    if (material) {
        material->setColor(color);
        addMaterial(material);
    }
    
    return material;
}

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(std::shared_ptr<TextureBase> texture) {
    ObjectId objId = getIdAndIncrement();
    std::shared_ptr<Material> material = Material::create(objId);;
    
    if (material) {
        material->setTexture(texture);
        addMaterial(material);
    }
    
    return material;
}

std::shared_ptr<BGE::Material> BGE::MaterialService::createMaterial(Vector4& color, std::shared_ptr<TextureBase> texture) {
    ObjectId objId = getIdAndIncrement();
    std::shared_ptr<Material> material = Material::create(objId);;
    
    if (material) {
        material->setColor(color);
        material->setTexture(texture);
        addMaterial(material);
    }
    
    return material;
}

void BGE::MaterialService::addMaterial(std::shared_ptr<Material> material) {
    MaterialMapIterator it = materials_.find(material->getInstanceId());
    
    if (it != materials_.end()) {
        // We already have the material
        // TODO: Error, warning, override?
    } else {
        materials_[material->getInstanceId()] = material;
    }

}

void BGE::MaterialService::removeMaterial(ObjectId matId) {
    materials_.erase(matId);
}

std::shared_ptr<BGE::Material> BGE::MaterialService::materialWithId(ObjectId matId) {
    if (materials_.find(matId) != materials_.end()) {
        return materials_[matId];
    } else {
        return nullptr;
    }
}
