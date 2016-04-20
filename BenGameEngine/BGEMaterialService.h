//
//  BGEMaterialService.h
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
#include "BGEService.h"
#include "BGEMathTypes.h"
#include "BGEMaterial.h"
#include "BGETexture.h"

class BGEMaterialService : BGEService
{
public:
    BGEMaterialService();
    ~BGEMaterialService();
    
    void initialize();
    void reset();
    void enteringBackground();
    void enteringForeground();
    void pause();
    void resume();
    void destroy();

    std::shared_ptr<BGEMaterial> createMaterial(std::string name, BGEVector4& color);
    std::shared_ptr<BGEMaterial> createMaterial(std::string name, std::shared_ptr<BGETextureBase> texture);
    std::shared_ptr<BGEMaterial> createMaterial(std::string name, BGEVector4& color, std::shared_ptr<BGETextureBase> texture);
    
    void removeMaterial(std::string name);
    
    std::shared_ptr<BGEMaterial> materialWithName(std::string name);
    
private:
    typedef std::unordered_map<std::string, std::shared_ptr<BGEMaterial>> MaterialMap;
    typedef std::unordered_map<std::string, std::shared_ptr<BGEMaterial>>::iterator MaterialMapIterator;

    MaterialMap materials_;
    
    void addMaterial(std::shared_ptr<BGEMaterial> material);
};

#endif /* BGEMaterialService_h */
