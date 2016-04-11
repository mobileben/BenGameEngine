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

    void addMaterial(std::string name, BGEVector4& color);
    void addMaterial(std::string name, std::shared_ptr<BGETexture> texture);
    void addMaterial(std::string name, BGEVector4& color, std::shared_ptr<BGETexture> texture);
    
    void removeMaterial(std::string name);
    
    std::shared_ptr<BGEMaterial> materialWithName(std::string name);
    
private:
    std::unordered_map<std::string, std::shared_ptr<BGEMaterial>> materials_;
};

#endif /* BGEMaterialService_h */
