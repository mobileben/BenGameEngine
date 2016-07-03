//
//  FontInfo.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/25/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef FontInfo_h
#define FontInfo_h

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "Font.h"

namespace BGE {
    
    struct FontInfo
    {
        std::string family;
        std::string style;
        std::string asset;
        uint32_t    faceIndex;
        
        std::string name() const {
            return family + " " + style;
        }
        
        std::unordered_map<uint32_t, FontHandle> fonts;
    };
}

#endif /* FontInfo_h */
