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
#include "TextureBase.h"
#include "Texture.h"
#include "TextureAtlas.h"
#include "Error.h"

namespace BGE {
    class TextureService : public Service
    {
    public:
        TextureService();
        virtual ~TextureService() {}
        
        virtual void namedTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback) =0;
        virtual void namedTextureFromURL(std::string name, std::string url, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback) =0;
        virtual void namedTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback) =0;
        
        virtual void namedTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback) =0;
        
        virtual std::shared_ptr<Texture> namedSubTexture(std::string name, std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) =0;
        void update(double deltaTime) {}

        void removeTexture(std::string name);
        void removeTexture(ObjectId texId);
        void removeTexture(std::shared_ptr<TextureBase>);
        
        // These methods will only return existing textures and is a synchronous call. This is for use in linking of packages post resource loading
        std::shared_ptr<TextureBase> textureWithName(std::string name);
        std::shared_ptr<TextureBase> textureWithId(ObjectId texId);
        
    protected:
        std::unordered_map<std::string, std::shared_ptr<TextureBase>> sTextures_;
        std::unordered_map<ObjectId, std::shared_ptr<TextureBase>> iTextures_;
    };
}

#endif /* BGETextureService_h */
