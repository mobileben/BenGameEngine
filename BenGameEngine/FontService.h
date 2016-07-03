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
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include "Service.h"
#include "FontInfo.h"
#include "Font.h"
#include "Error.h"
#include "Handle.h"
#include "HandleService.h"
#include <Foundation/Foundation.h>

namespace BGE {
    class FontService : public Service
    {
    public:
        
        static std::string fontAsKey(std::string name, uint32_t pixelSize);
        static void mapBundles(std::string bundleName);
        
        FontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        ~FontService() {}
        
        FontHandle getFontHandle(ObjectId fontId);
        FontHandle getFontHandle(std::string name, uint32_t pixelSize);

        Font *getFont(ObjectId fontId);
        Font *getFont(std::string name, uint32_t pixelSize);
        Font *getFont(FontHandle handle);
        
        void removeFont(ObjectId fontId);
        void removeFont(std::string name, uint32_t pixelSize);
        void removeFont(FontHandle handle);
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}

        void loadFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);
        void unloadFont(FontHandle handle);
        void unloadFont(std::string name, uint32_t pixelSize);
 
    protected:
        // TODO: For now this is Mac/iOS specific
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
        
        std::unordered_map<std::string, std::string>                fontResources_;
        std::unordered_map<std::string, std::shared_ptr<FontInfo>>  fontTable_;
        std::vector<std::shared_ptr<FontInfo>>                      fontInfo_;
        
        std::string pathForAsset(std::string asset);
        
    private:
        static const uint32_t InitialFontReserve = 32;

        using FontHandleService = HandleService<Font, FontHandle>;
        
        FontHandleService handleService_;

        void buildFontInfoForAsset(std::string asset);
    };
}


#endif /* BGEFontService_h */
