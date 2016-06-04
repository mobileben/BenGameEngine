//
//  FontService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FontService.h"
#include <cassert>

std::string BGE::FontService::fontAsKey(std::string name, uint32_t pixelSize) {
    return name + std::to_string(pixelSize);
}

BGE::FontService::FontService(std::map<std::string, std::string> resources) {
    FT_Error error = FT_Init_FreeType(&freetypeLibrary_);
    
    assert(!error);
    
    if (!error) {
        fontResources_["default"] = "Avenir.ttc";
        fontResources_["Avenir Black"] = "Avenir.ttc";
        fontResources_.insert(resources.begin(), resources.end());
    }
}
