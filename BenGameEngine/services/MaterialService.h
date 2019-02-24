//
//  MaterialService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEMaterialService_h
#define BGEMaterialService_h

#include <stdio.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "Service.h"
#include "MathTypes.h"
#include "Material.h"
#include "Texture.h"
#include "HandleService.h"

namespace BGE {
    class MaterialService : public Service
    {
    public:
        MaterialService();
        ~MaterialService() {}
        
        void initialize() final;
        void reset() final;
        void platformSuspending() final;
        void platformResuming() final;
        void pause() final;
        void resume() final;
        void destroy() final;
        void update(__attribute__ ((unused)) double deltaTime) final {}
        void garbageCollect() final { handleService_.garbageCollect(); }
        
        uint32_t numMaterials() const;
        
        uint32_t numUsedHandles() const final;
        uint32_t maxHandles() const final;
        uint32_t numHandleResizes() const final;
        uint32_t maxHandlesAllocated() const final;
        
        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;

        MaterialHandle createMaterial(Color& color);
        MaterialHandle createMaterial(Color&& color);
        MaterialHandle createMaterial(TextureHandle textureHandle);
        MaterialHandle createMaterial(Color& color, TextureHandle textureHandle);
        MaterialHandle createMaterial(Color&& color, TextureHandle textureHandle);
        
        void removeMaterial(MaterialHandle handle);
        void removeMaterial(ObjectId matId);
        
        MaterialHandle getMaterialHandle(ObjectId matId) const;

        Material *getMaterial(MaterialHandle handle) const;
        Material *getMaterialLockless(MaterialHandle handle) const;
        Material *getMaterial(ObjectId matId) const;
        Material *getMaterialLockless(ObjectId matId) const;

    private:
        static const uint32_t InitialMaterialReserve = 2048;
        
        using MaterialHandleService = HandleService<Material, MaterialHandle>;
        
        using MaterialMap = std::unordered_map<ObjectId, MaterialHandle>;
        using MaterialMapIterator = MaterialMap::iterator;
        
        MaterialHandleService   handleService_;
        MaterialMap             materials_;
        
        void addMaterial(MaterialHandle material);
    };
}

#endif /* BGEMaterialService_h */
