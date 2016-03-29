//
//  BGEFontService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGEFontService.h"
#include <cassert>

std::string BGEFontService::fontAsKey(std::string name, uint32_t pixelSize) {
    return name + std::to_string(pixelSize);
}

BGEFontService::BGEFontService(std::map<std::string, std::string> resources) {
    FT_Error error = FT_Init_FreeType(&freetypeLibrary_);
    
    assert(!error);
    
    if (!error) {
        fontResources_["default"] = "Avenir.ttc";
        fontResources_.insert(resources.begin(), resources.end());
    }
}
