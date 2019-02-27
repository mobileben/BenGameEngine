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

void BGE::MaterialService::initialize() {}
void BGE::MaterialService::reset() {}
void BGE::MaterialService::platformSuspending() { Service::platformSuspending(); }
void BGE::MaterialService::platformResuming() { Service::platformResuming(); }
void BGE::MaterialService::pause() { Service::pause(); }
void BGE::MaterialService::resume() { Service::resume(); }
void BGE::MaterialService::destroy() {}

BGE::MaterialHandle BGE::MaterialService::createMaterial(Color& color) {
    MaterialHandle handle;
    Material *material = handleService_.allocate(handle);
    
    if (material) {
        material->initialize(handle);
        material->setColor(color);
        addMaterial(handle);
    }
    
    return handle;
}

BGE::MaterialHandle BGE::MaterialService::createMaterial(Color&& color) {
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

BGE::MaterialHandle BGE::MaterialService::createMaterial(Color& color, TextureHandle textureHandle) {
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

BGE::MaterialHandle BGE::MaterialService::createMaterial(Color&& color, TextureHandle textureHandle) {
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
        lock();
        MaterialMapIterator it = materials_.find(material->getInstanceId());
        
        if (it != materials_.end()) {
            // We already have the material
            // TODO: Error, warning, override?
        } else {
            materials_[material->getInstanceId()] = handle;
        }
        unlock();
    } else {
        // TODO: Error, what do we do?
    }
}

void BGE::MaterialService::removeMaterial(MaterialHandle handle) {
    auto material = getMaterial(handle);
    
    if (material) {
        lock();
        MaterialMapIterator it = materials_.find(material->getInstanceId());
        
        if (it != materials_.end()) {
            material->destroy();
            materials_.erase(it);
            unlock();
            handleService_.release(handle);
        } else {
            unlock();
        }
    }
}

void BGE::MaterialService::removeMaterial(ObjectId matId) {
    lock();
    MaterialMapIterator it = materials_.find(matId);
    
    if (it != materials_.end()) {
        auto material = getMaterial(it->second);
        
        material->destroy();
        materials_.erase(it);
        unlock();
        handleService_.release(it->second);
    } else {
        unlock();
    }
}

BGE::MaterialHandle BGE::MaterialService::getMaterialHandle(ObjectId matId) const {
    lock();
    auto it = materials_.find(matId);

    if (it != materials_.end()) {
        unlock();
        return it->second;
    } else {
        unlock();
        return MaterialHandle();
    }
}

BGE::Material *BGE::MaterialService::getMaterial(MaterialHandle handle) const {
    return handleService_.dereference(handle);
}

BGE::Material *BGE::MaterialService::getMaterialLockless(MaterialHandle handle) const {
    return handleService_.dereferenceLockless(handle);
}

BGE::Material *BGE::MaterialService::getMaterial(ObjectId matId) const {
    lock();
    auto it = materials_.find(matId);
    
    if (it != materials_.end()) {
        unlock();
        return handleService_.dereference(it->second);
    }
    unlock();

    return nullptr;
}

BGE::Material *BGE::MaterialService::getMaterialLockless(ObjectId matId) const {
    lock();
    auto it = materials_.find(matId);
    
    if (it != materials_.end()) {
        unlock();
        return handleService_.dereferenceLockless(it->second);
    }
    unlock();
    return nullptr;
}

uint32_t BGE::MaterialService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::MaterialService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::MaterialService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::MaterialService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

uint32_t BGE::MaterialService::numMaterials() const {
    return handleService_.numUsedHandles();
}

size_t BGE::MaterialService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::MaterialService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::MaterialService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

