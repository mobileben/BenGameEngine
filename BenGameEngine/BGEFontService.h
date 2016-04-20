//
//  BGEFontService.h
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
#include "BGEService.h"
#include "BGEFont.h"
#include "Error.h"
#include "ft2build.h"
#include FT_FREETYPE_H

class BGEFontService : public BGEService
{
public:
    static std::string fontAsKey(std::string name, uint32_t pixelSize);
    
    BGEFontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
    virtual ~BGEFontService() {}
    
    FT_Library getFreetypeLibrary() const { return freetypeLibrary_; }
    virtual std::shared_ptr<BGEFont> getFont(std::string name, uint32_t pixelSize) =0;

    virtual void loadFont(std::string name, uint32_t pxSize, std::function<void(std::shared_ptr<BGEFont>, std::shared_ptr<BGE::Error> error)> callback) =0;
    virtual void unloadFont(std::string name, uint32_t pixelSize) =0;

protected:
    FT_Library freetypeLibrary_;
    
    std::map<std::string, std::string> fontResources_;
    std::map<std::string, std::shared_ptr<BGEFont>> fonts_;
};

#endif /* BGEFontService_h */
