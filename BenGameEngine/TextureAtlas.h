//
//  TextureAtlas.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureAtlas_h
#define BGETextureAtlas_h

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "TextureBase.h"
#include "Texture.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;
} BGESubTextureDef;

namespace BGE {
    class TextureAtlas : public TextureBase
    {
    public:
        TextureAtlas(ObjectId objId, std::string name);
        
        virtual uint32_t getHWTextureId() const =0;
        
        uint32_t getWidth() const { return texture_ ? texture_->getWidth() : 0; }
        uint32_t getHeight() const { return texture_ ? texture_->getHeight() : 0; }
        
        std::string atlasTextureKey() const;
        std::shared_ptr<Texture> getTexture() const { return texture_; };
        std::shared_ptr<Texture> getSubTexture(std::string name);
        
        virtual void createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback) =0;
        
        void releaseCurrentTexture();
        
    protected:
        std::string textureName_;
        
        // TODO: Make weak?
        std::shared_ptr<Texture> texture_;
        // TODO: Make weak?
        std::map<std::string, std::shared_ptr<Texture>> subTextures_;
    };
}

#endif /* BGETextureAtlas_h */
