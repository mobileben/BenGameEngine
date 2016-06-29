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
#include "FontInfo.h"
#include "Font.h"
#include "Error.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <Foundation/Foundation.h>

namespace BGE {
    class FontService : public BGE::Service
    {
    public:
        static std::string fontAsKey(std::string name, uint32_t pixelSize);
        static void mapBundles(std::string bundleName);
        
        FontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        virtual ~FontService() {}
        
        std::shared_ptr<Font> getFont(std::string name, uint32_t pixelSize);
        
        virtual void initialize() {}
        virtual void reset() {}
        virtual void enteringBackground() {}
        virtual void enteringForeground() {}
        virtual void pause() {}
        virtual void resume() {}
        virtual void destroy() {}
        void update(double deltaTime) {}

        void loadFont(std::string name, uint32_t pxSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<Error> error)> callback);
        void unloadFont(std::string name, uint32_t pixelSize);
        
    protected:
        // TODO: For now this is Mac/iOS specific
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
        
        std::map<std::string, std::string> fontResources_;
        std::map<std::string, std::shared_ptr<FontInfo>> fontTable_;
        std::vector<std::shared_ptr<FontInfo>> fontInfo_;
        
        std::string pathForAsset(std::string asset);
        
    private:
        void buildFontInfoForAsset(std::string asset);
    };
}

#endif /* BGEFontService_h */
