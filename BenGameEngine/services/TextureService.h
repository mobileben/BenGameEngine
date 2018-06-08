//
//  TextureService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureService_h
#define BGETextureService_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif /* TARGET_OS_IPHONE */

#include <stdio.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <mutex>

#include "Service.h"
#include "Texture.h"
#include "TextureAtlas.h"
#include "Error.h"
#include "Handle.h"
#include "HandleService.h"
#if TARGET_OS_IPHONE
#include <GLKit/GLKit.h>
#endif /* TARGET_OS_IPHONE */

namespace BGE {
    class TextureService : public Service
    {
    public:
#if TARGET_OS_IPHONE
        TextureService(EAGLContext *context);
#else
        TextureService();
#endif /* TARGET_OS_IPHONE */
        ~TextureService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) {}
        void garbageCollect() final;
        
        uint32_t numTextures();   // Texture count includes subtextures
        uint32_t numSubTextures();
        uint32_t numTextureAtlases();
        
        uint32_t numUsedTextureHandles() const;
        uint32_t maxTextureHandles() const;
        uint32_t numTextureHandleResizes() const;
        uint32_t maxTextureHandlesAllocated() const;
        
        uint32_t numUsedTextureAtlasHandles() const;
        uint32_t maxTextureAtlasHandles() const;
        uint32_t numTextureAtlasHandleResizes() const;
        uint32_t maxTextureAtlasHandlesAllocated() const;

        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;

        size_t totalTextureMemory();
        
        void outputMemoryBreakdown(uint32_t numTabs);
        
        TextureHandle getTextureHandle(ScenePackageHandle scenePackageHandle, std::string name);
        TextureHandle getTextureHandle(SpaceHandle spaceHandle, std::string name);
        TextureHandle getTextureHandle(TextureAtlasHandle atlasHandle, std::string name);
        
        TextureAtlasHandle getTextureAtlasHandle(ScenePackageHandle scenePackageHandle, std::string name);
        TextureAtlasHandle getTextureAtlasHandle(SpaceHandle spaceHandle, std::string name);
        TextureAtlasHandle getTextureAtlasHandle(FontHandle fontHandle, std::string name);

        Texture *getTexture(ScenePackageHandle scenePackageHandle, std::string name);
        Texture *getTexture(SpaceHandle spaceHandle, std::string name);
        Texture *getTexture(TextureAtlasHandle atlasHandle, std::string name);
        inline Texture *getTexture(TextureHandle handle) {
            return textureHandleService_.dereference(handle);
        }

        TextureAtlas *getTextureAtlas(ScenePackageHandle scenePackageHandle, std::string name);
        TextureAtlas *getTextureAtlas(SpaceHandle spaceHandle, std::string name);
        TextureAtlas *getTextureAtlas(FontHandle fontHandle, std::string name);
        inline TextureAtlas *getTextureAtlas(TextureAtlasHandle handle) {
            return textureAtlasHandleService_.dereference(handle);
        }

        void removeTexture(ScenePackageHandle scenePackageHandle, TextureHandle handle);
        void removeTexture(SpaceHandle spaceHandle, TextureHandle handle);
        
        void removeTextureAtlas(ScenePackageHandle scenePackageHandle, TextureAtlasHandle handle);
        void removeTextureAtlas(SpaceHandle spaceHandle, TextureAtlasHandle handle);
        void removeTextureAtlas(FontHandle fontHandle, TextureAtlasHandle handle);
        
    protected:
        friend class Font;
        friend class ScenePackage;
        friend class Space;
        friend class Texture;
        friend class TextureAtlas;
        
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, TextureFormat format);
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height);
        
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, TextureFormat format);
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height);
        
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromFile(TextureAtlasHandle atlasHandle, std::string name, std::string filename, TextureFormat format);
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromBuffer(TextureAtlasHandle atlasHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height);

#if TARGET_OS_IPHONE
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromUIImage(SpaceHandle spaceHandle, const std::string& name, UIImage *image);
#endif /* TARGET_OS_IPHONE */

        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format);
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs);
        
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format);
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs);
        
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromFile(FontHandle fontHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format);
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromBuffer(FontHandle fontHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs);
        
        Texture *createSubTexture(TextureAtlasHandle atlasHandle, std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated = false);
    
    private:
        static const uint32_t InitialTextureReserve = 4096;
        static const uint32_t InitialTextureAtlasReserve = 32;
        
        using TextureHandleService = HandleService<Texture, TextureHandle>;
        using TextureAtlasHandleService = HandleService<TextureAtlas, TextureAtlasHandle>;
        
        TextureHandleService        textureHandleService_;
        TextureAtlasHandleService   textureAtlasHandleService_;
        
#ifdef SUPPORT_GLKTEXTURELOADER
        GLKTextureLoader            *textureLoader_;
#endif
        
        std::unordered_map<ScenePackageHandle, std::unordered_map<std::string, TextureHandle>> packageTextures_;
        std::unordered_map<SpaceHandle, std::unordered_map<std::string, TextureHandle>> spaceTextures_;
        std::unordered_map<TextureAtlasHandle, std::unordered_map<std::string, TextureHandle>> atlasTextures_;
        
        std::unordered_map<ScenePackageHandle, std::unordered_map<std::string, TextureAtlasHandle>> packageTextureAtlases_;
        std::unordered_map<SpaceHandle, std::unordered_map<std::string, TextureAtlasHandle>> spaceTextureAtlases_;
        std::unordered_map<FontHandle, std::unordered_map<std::string, TextureAtlasHandle>> fontTextureAtlases_;

        // Mutex for each map above
        std::mutex  packageTexturesMutex_;
        std::mutex  spaceTexturesMutex_;
        std::mutex  atlasTexturesMutex_;
        std::mutex  packageTextureAtlasesMutex_;
        std::mutex  spaceTextureAtlasesMutex_;
        std::mutex  fontTextureAtlasesMutex_;

        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromFile(std::string name, std::string filename, TextureFormat format);
        std::pair<Texture *, std::shared_ptr<Error>> createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height);

        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format);
        std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs);
        BGE::Texture *createSubTexture(std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated);
        
        void removeTexture(TextureAtlasHandle atlasHandle, TextureHandle handle);

        void releaseTexture(Texture *texture);
        void releaseTextureAtlas(TextureAtlas *atlas);
    };
}

#endif /* BGETextureService_h */
