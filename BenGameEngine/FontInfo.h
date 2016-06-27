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
#include <map>

namespace BGE {
    class Font;
    
    struct FontInfo
    {
        std::string family;
        std::string style;
        std::string asset;
        uint32_t    faceIndex;
        
        std::string name() const {
            return family + " " + style;
        }
        
        std::map<uint32_t, std::shared_ptr<Font>> fonts;
    };
    
    bool operator==(const std::string& lhs, const FontInfo& rhs);
    bool operator!=(const std::string& lhs, const FontInfo& rhs);
    bool operator==(const FontInfo& lhs, const std::string& rhs);
    bool operator!=(const FontInfo& lhs, const std::string& rhs);
}

#endif /* FontInfo_h */
