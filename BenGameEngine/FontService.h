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
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}

        uint32_t numFonts() const;
        
        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;

        void removeFont(ScenePackageHandle scenePackageHandle, FontHandle handle);
        void removeFont(SpaceHandle spaceHandle, FontHandle handle);

        FontHandle getFontHandle(std::string name, uint32_t pixelSize);
        
        Font *getFont(std::string name, uint32_t pixelSize);
        Font *getFont(FontHandle handle);
 
    protected:
        // TODO: For now this is Mac/iOS specific
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
        
        using FontTableMap = std::unordered_map<std::string, std::shared_ptr<FontInfo>>;
        
        std::unordered_map<std::string, std::string>                    fontResources_;
        FontTableMap                                                    fontTable_;
        std::unordered_map<FontHandle, std::vector<ScenePackageHandle>> fontScenePackages_;
        std::unordered_map<FontHandle, std::vector<SpaceHandle>>        fontSpaces_;
        std::vector<std::shared_ptr<FontInfo>>                          fontInfo_;
        
        std::string pathForAsset(std::string asset);

        void createFont(std::string name, uint32_t pxSize, ScenePackageHandle scenePackageHandle, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);
        void createFont(std::string name, uint32_t pxSize, SpaceHandle spaceHandle, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);

    private:
        friend class ScenePackage;
        friend class Space;
        
        static const uint32_t InitialFontReserve = 32;

        using FontHandleService = HandleService<Font, FontHandle>;
        
        void createFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);
        FontHandleService handleService_;
        void removeFont(FontHandle handle);

        void buildFontInfoForAsset(std::string asset);
        
        bool fontHasReferences(FontHandle fontHandle);
    };
}


#endif /* BGEFontService_h */
