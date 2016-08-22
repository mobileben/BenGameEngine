//
//  MaterialService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MaterialService.h"

BGE::MaterialService::MaterialService() :  handleService_(InitialMaterialReserve, HandleServiceNoMaxLimit) {
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

BGE::MaterialHandle BGE::MaterialService::createMaterial(Vector4& color) {
    MaterialHandle handle;
    Material *material = handleService_.allocate(handle);
    
    if (material) {
        material->initialize(handle);
        material->setColor(color);
        addMaterial(handle);
    }
    
    return handle;
}

BGE::MaterialHandle BGE::MaterialService::createMaterial(TextureHandle textureHandle) {
    MaterialHandle handle;
    Material *material = handleService_.allocate(handle);
    
    if (material) {
        material->initialize(handle);
        material->setTextureHandle(textureHandle);
        addMaterial(handle);
    }
    
    return handle;
}

BGE::MaterialHandle BGE::MaterialService::createMaterial(Vector4& color, TextureHandle textureHandle) {
    MaterialHandle handle;
    Material *material = handleService_.allocate(handle);
    
    if (material) {
        material->initialize(handle);
        material->setColor(color);
        material->setTextureHandle(textureHandle);
        addMaterial(handle);
    }
    
    return handle;
}

void BGE::MaterialService::addMaterial(MaterialHandle handle) {
    auto material = getMaterial(handle);
    
    if (material) {
        MaterialMapIterator it = materials_.find(material->getInstanceId());
        
        if (it != materials_.end()) {
            // We already have the material
            // TODO: Error, warning, override?
        } else {
            materials_[material->getInstanceId()] = handle;
        }
    } else {
        // TODO: Error, what do we do?
    }
}

void BGE::MaterialService::removeMaterial(MaterialHandle handle) {
    auto material = getMaterial(handle);
    
    if (material) {
        MaterialMapIterator it = materials_.find(material->getInstanceId());
        
        if (it != materials_.end()) {
            handleService_.release(handle);
            materials_.erase(material->getInstanceId());
        }
    }
}

void BGE::MaterialService::removeMaterial(ObjectId matId) {
    MaterialMapIterator it = materials_.find(matId);
    
    if (it != materials_.end()) {
        handleService_.release(it->second);
        materials_.erase(matId);
    }
}

BGE::MaterialHandle BGE::MaterialService::getMaterialHandle(ObjectId matId) {
    if (materials_.find(matId) != materials_.end()) {
        return materials_[matId];
    } else {
        return MaterialHandle();
    }
}

BGE::Material *BGE::MaterialService::getMaterial(MaterialHandle handle) {
    return handleService_.dereference(handle);
}

BGE::Material *BGE::MaterialService::getMaterial(ObjectId matId) {
    MaterialMapIterator it = materials_.find(matId);
    
    if (it != materials_.end()) {
        return handleService_.dereference(it->second);
    }

    return nullptr;
}

