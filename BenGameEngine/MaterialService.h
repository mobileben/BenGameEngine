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
#include "BGEMathTypes.h"
#include "Material.h"
#include "BGETexture.h"

namespace BGE {
    class MaterialService : public BGE::Service
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
        
        std::shared_ptr<Material> createMaterial(std::string name, BGEVector4& color);
        std::shared_ptr<Material> createMaterial(std::string name, std::shared_ptr<BGETextureBase> texture);
        std::shared_ptr<Material> createMaterial(std::string name, BGEVector4& color, std::shared_ptr<BGETextureBase> texture);
        
        void removeMaterial(std::string name);
        
        std::shared_ptr<Material> materialWithName(std::string name);
        
    private:
        typedef std::unordered_map<std::string, std::shared_ptr<Material>> MaterialMap;
        typedef std::unordered_map<std::string, std::shared_ptr<Material>>::iterator MaterialMapIterator;
        
        MaterialMap materials_;
        
        void addMaterial(std::shared_ptr<Material> material);
    };
}

#endif /* BGEMaterialService_h */
