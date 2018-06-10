//
//  FontService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontService_h
#define BGEFontService_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

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
#if TARGET_OS_IPHONE
#include <Foundation/Foundation.h>
#endif /* TARGET_OS_IPHONE */

namespace BGE {
    class FontService : public Service
    {
    public:
        static std::string fontAsKey(std::string name, uint32_t pixelSize);
#if TARGET_OS_IPHONE
        static void mapBundles(std::string bundleName);
#endif /* TARGET_OS_IPHONE */
        
        FontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        ~FontService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final {}
        void garbageCollect() final { handleService_.garbageCollect(); }
        
        uint32_t numFonts() const;
        
        uint32_t numUsedHandles() const final;
        uint32_t maxHandles() const final;
        uint32_t numHandleResizes() const final;
        uint32_t maxHandlesAllocated() const final;

        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;

        void removeFont(ScenePackageHandle scenePackageHandle, FontHandle handle);
        void removeFont(SpaceHandle spaceHandle, FontHandle handle);

        FontHandle getFontHandle(std::string name, uint32_t pixelSize) const;
        
        Font *getFont(std::string name, uint32_t pixelSize) const;
        Font *getFont(FontHandle handle) const;
 
    protected:
#if TARGET_OS_IPHONE
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
#endif /* TARGET_OS_IPHONE */
        
        using FontTableMap = std::unordered_map<std::string, std::shared_ptr<FontInfo>>;
        
        std::unordered_map<std::string, std::string>                    fontResources_;
        FontTableMap                                                    fontTable_;
        std::unordered_map<FontHandle, std::vector<ScenePackageHandle>> fontScenePackages_;
        std::unordered_map<FontHandle, std::vector<SpaceHandle>>        fontSpaces_;
        std::vector<std::shared_ptr<FontInfo>>                          fontInfo_;
        
        std::string pathForAsset(std::string asset);

        std::pair<FontHandle, std::shared_ptr<Error>> createFont(std::string name, uint32_t pxSize, ScenePackageHandle scenePackageHandle);
        std::pair<FontHandle, std::shared_ptr<Error>> createFont(std::string name, uint32_t pxSize, SpaceHandle spaceHandle);

    private:
        friend class ScenePackage;
        friend class Space;
        
        static const uint32_t InitialFontReserve = 32;

        using FontHandleService = HandleService<Font, FontHandle>;

        FontHandleService handleService_;

        std::pair<FontHandle, std::shared_ptr<Error>> createFont(std::string name, uint32_t pxSize);
        void removeFont(FontHandle handle);

        void buildFontInfoForAsset(std::string asset);
        
        bool fontHasReferences(FontHandle fontHandle);
    };
}


#endif /* BGEFontService_h */
