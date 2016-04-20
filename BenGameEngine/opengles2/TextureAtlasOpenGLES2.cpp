//
//  TextureAtlasOpenGLES2.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureAtlasOpenGLES2.h"
#include "TextureOpenGLES2.h"
#include "Game.h"

BGE::TextureAtlasOpenGLES2::TextureAtlasOpenGLES2(uint64_t texId, std::string name) : TextureAtlas(texId ,name) {
}

BGE::TextureAtlasOpenGLES2::~TextureAtlasOpenGLES2() {
}

uint32_t BGE::TextureAtlasOpenGLES2::getHWTextureId() const {
    if (texture_) {
        return texture_->getHWTextureId();
    } else {
        return 0;
    }
}

GLenum BGE::TextureAtlasOpenGLES2::getTarget() const {
    std::shared_ptr<BGE::TextureOpenGLES2> oglTexture = std::dynamic_pointer_cast<BGE::TextureOpenGLES2>(texture_);
    
    if (oglTexture) {
        return oglTexture->getTarget();
    } else {
        return 0;
    }
}

void BGE::TextureAtlasOpenGLES2::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<BGE::Error>)> callback) {
    releaseCurrentTexture();

    BGE::Game::getInstance()->getTextureService()->namedTextureFromBuffer(atlasTextureKey(), buffer, format, width, height, [this, &subTextures, &callback](std::shared_ptr<BGE::TextureBase> atlas, std::shared_ptr<BGE::Error> error) {
        if (!error) {
            std::shared_ptr<BGE::TextureAtlas> a = std::dynamic_pointer_cast<BGE::TextureAtlas>(shared_from_this());
            std::shared_ptr<BGE::Texture> subTex;
            std::shared_ptr<BGE::Error> bgeError;
            std::shared_ptr<BGE::Texture> texture;
            
            texture = std::dynamic_pointer_cast<Texture>(atlas);
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->texture_ = texture;
                this->textureName_ = atlasTextureKey();
                
                if (subTextures.size() > 0) {
                    for (auto& kv : subTextures) {
                        std::string key = kv.first;
                        subTex = BGE::Game::getInstance()->getTextureService()->namedSubTexture(kv.first, a, kv.second.x, kv.second.y, kv.second.width, kv.second.height);
                        
                        if (subTex) {
                            subTextures_[key] = subTex;
                        }
                    }
                    
                    
                    this->valid_ = true;
                    this->format_ = texture->getFormat();
                    this->alphaState_ = texture->getAlphaState();
                    this->width_ = texture->getWidth();
                    this->height_ = texture->getHeight();
                } else {
                    a.reset();
                    bgeError = std::make_shared<BGE::Error>(BGE::TextureBase::ErrorDomain, TextureErrorInvalidSubTexture);
                }
            } else {
                a.reset();
                bgeError = std::make_shared<BGE::Error>(BGE::TextureBase::ErrorDomain, TextureErrorExistingTextureWrongType);
            }
            
            
            if (callback) {
                callback(a, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}
