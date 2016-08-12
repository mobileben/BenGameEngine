//
//  TextureService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureService_h
#define BGETextureService_h

#include <stdio.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include "Service.h"
#include "Texture.h"
#include "TextureAtlas.h"
#include "Error.h"
#include "Handle.h"
#include "HandleService.h"
#include <GLKit/GLKit.h>

namespace BGE {
    class TextureService : public Service
    {
    public:
        TextureService(EAGLContext *context);
        ~TextureService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}

        void createTextureFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback);

        void createTextureFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        
        void createTextureFromFile(TextureAtlasHandle atlasHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureFromBuffer(TextureAtlasHandle atlasHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback);

        void createTextureAtlasFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        
        void createTextureAtlasFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);

        void createTextureAtlasFromFile(FontHandle fontHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromBuffer(FontHandle fontHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        
        Texture *createSubTexture(TextureAtlasHandle atlasHandle, std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated = false);

        TextureHandle getTextureHandle(ScenePackageHandle scenePackageHandle, std::string name);
        TextureHandle getTextureHandle(SpaceHandle spaceHandle, std::string name);
        TextureHandle getTextureHandle(TextureAtlasHandle atlasHandle, std::string name);
        
        TextureAtlasHandle getTextureAtlasHandle(ScenePackageHandle scenePackageHandle, std::string name);
        TextureAtlasHandle getTextureAtlasHandle(SpaceHandle spaceHandle, std::string name);
        TextureAtlasHandle getTextureAtlasHandle(FontHandle fontHandle, std::string name);
        
        Texture *getTexture(ScenePackageHandle scenePackageHandle, std::string name);
        Texture *getTexture(SpaceHandle spaceHandle, std::string name);
        Texture *getTexture(TextureAtlasHandle atlasHandle, std::string name);
        
        TextureAtlas *getTextureAtlas(ScenePackageHandle scenePackageHandle, std::string name);
        TextureAtlas *getTextureAtlas(SpaceHandle spaceHandle, std::string name);
        TextureAtlas *getTextureAtlas(FontHandle fontHandle, std::string name);

        Texture *getTexture(TextureHandle handle) {
            return textureHandleService_.dereference(handle);            
        }
        
        TextureAtlas *getTextureAtlas(TextureAtlasHandle handle) {
            return textureAtlasHandleService_.dereference(handle);
        }

        void removeTexture(ScenePackageHandle scenePackageHandle, TextureHandle handle);
        void removeTexture(SpaceHandle spaceHandle, TextureHandle handle);
        
        void removeTextureAtlas(ScenePackageHandle scenePackageHandle, TextureAtlasHandle handle);
        void removeTextureAtlas(SpaceHandle spaceHandle, TextureAtlasHandle handle);
        void removeTextureAtlas(FontHandle fontHandle, TextureAtlasHandle handle);
        
    protected:
        static const uint32_t InitialTextureReserve = 1024;
        static const uint32_t InitialTextureAtlasReserve = 32;
        
        using TextureHandleService = HandleService<Texture, TextureHandle>;
        using TextureAtlasHandleService = HandleService<TextureAtlas, TextureAtlasHandle>;
        
        TextureHandleService        textureHandleService_;
        TextureAtlasHandleService   textureAtlasHandleService_;
        
        GLKTextureLoader            *textureLoader_;

        std::unordered_map<ScenePackageHandle, std::unordered_map<std::string, TextureHandle>> sceneTextures_;
        std::unordered_map<SpaceHandle, std::unordered_map<std::string, TextureHandle>> spaceTextures_;
        std::unordered_map<TextureAtlasHandle, std::unordered_map<std::string, TextureHandle>> atlasTextures_;
        
        std::unordered_map<ScenePackageHandle, std::unordered_map<std::string, TextureAtlasHandle>> sceneTextureAtlases_;
        std::unordered_map<SpaceHandle, std::unordered_map<std::string, TextureAtlasHandle>> spaceTextureAtlases_;
        std::unordered_map<FontHandle, std::unordered_map<std::string, TextureAtlasHandle>> fontTextureAtlases_;
        
        void createTextureFromFile(std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback);

        void createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        BGE::Texture *createSubTexture(std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated);
    };
}

#endif /* BGETextureService_h */
