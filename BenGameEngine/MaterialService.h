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
        ~MaterialService();
        
        void initialize();
        void reset();
        void enteringBackground();
        void enteringForeground();
        void pause();
        void resume();
        void destroy();
        void update(double deltaTime) {}

        MaterialHandle createMaterial(Vector4& color);
        MaterialHandle createMaterial(TextureHandle textureHandle);
        MaterialHandle createMaterial(Vector4& color, TextureHandle textureHandle);
        
        void removeMaterial(MaterialHandle handle);
        void removeMaterial(ObjectId matId);
        
        MaterialHandle getMaterialHandle(ObjectId matId);
        
        Material *getMaterial(MaterialHandle handle);
        Material *getMaterial(ObjectId matId);
        
    private:
        static const uint32_t InitialMaterialReserve = 1024;
        
        using MaterialHandleService = HandleService<Material, MaterialHandle>;
        
        using MaterialMap = std::unordered_map<ObjectId, MaterialHandle>;
        using MaterialMapIterator = MaterialMap::iterator;
        
        MaterialHandleService   handleService_;
        MaterialMap             materials_;
        
        void addMaterial(MaterialHandle material);
    };
}

#endif /* BGEMaterialService_h */
