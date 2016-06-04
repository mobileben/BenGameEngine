//
//  FontService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontService_h
#define BGEFontService_h

#include <stdio.h>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "Service.h"
#include "Font.h"
#include "Error.h"
#include "ft2build.h"
#include FT_FREETYPE_H

namespace BGE {
    class FontService : public BGE::Service
    {
    public:
        static std::string fontAsKey(std::string name, uint32_t pixelSize);
        
        FontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        virtual ~FontService() {}
        
        FT_Library getFreetypeLibrary() const { return freetypeLibrary_; }
        virtual std::shared_ptr<Font> getFont(std::string name, uint32_t pixelSize) =0;
        
        virtual void loadFont(std::string name, uint32_t pxSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback) =0;
        virtual void unloadFont(std::string name, uint32_t pixelSize) =0;
        
    protected:
        FT_Library freetypeLibrary_;
        
        std::map<std::string, std::string> fontResources_;
        std::map<std::string, std::shared_ptr<Font>> fonts_;
        std::map<std::string, std::shared_ptr<Font>> underlyingFonts_;
    };
}

#endif /* BGEFontService_h */
