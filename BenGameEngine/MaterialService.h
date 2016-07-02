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

        std::shared_ptr<Material> createMaterial(Vector4& color);
        std::shared_ptr<Material> createMaterial(std::shared_ptr<TextureBase> texture);
        std::shared_ptr<Material> createMaterial(Vector4& color, std::shared_ptr<TextureBase> texture);
        
        void removeMaterial(ObjectId matId);
        
        std::shared_ptr<Material> materialWithId(ObjectId matId);
        
    private:
        using MaterialMap = std::unordered_map<ObjectId, std::shared_ptr<Material>>;
        using MaterialMapIterator = MaterialMap::iterator;
        
        MaterialMap materials_;
        
        void addMaterial(std::shared_ptr<Material> material);
    };
}

#endif /* BGEMaterialService_h */
