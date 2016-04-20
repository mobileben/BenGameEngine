//
//  BGETextureAtlasOpenGLES2.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETextureAtlasOpenGLES2.h"
#include "BGETextureOpenGLES2.h"
#include "BGEGame.h"

BGETextureAtlasOpenGLES2::BGETextureAtlasOpenGLES2(uint64_t texId, std::string name) : BGETextureAtlas(texId ,name) {
}

BGETextureAtlasOpenGLES2::~BGETextureAtlasOpenGLES2() {
}

uint32_t BGETextureAtlasOpenGLES2::getHWTextureId() const {
    if (texture_) {
        return texture_->getHWTextureId();
    } else {
        return 0;
    }
}

GLenum BGETextureAtlasOpenGLES2::getTarget() const {
    std::shared_ptr<BGETextureOpenGLES2> oglTexture = std::dynamic_pointer_cast<BGETextureOpenGLES2>(texture_);
    
    if (oglTexture) {
        return oglTexture->getTarget();
    } else {
        return 0;
    }
}

void BGETextureAtlasOpenGLES2::createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextures, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGE::Error>)> callback) {
    releaseCurrentTexture();

    BGEGame::getInstance()->getTextureService()->namedTextureFromBuffer(atlasTextureKey(), buffer, format, width, height, [this, &subTextures, &callback](std::shared_ptr<BGETextureBase> atlas, std::shared_ptr<BGE::Error> error) {
        if (!error) {
            std::shared_ptr<BGETextureAtlas> a = std::dynamic_pointer_cast<BGETextureAtlas>(shared_from_this());
            std::shared_ptr<BGETexture> subTex;
            std::shared_ptr<BGE::Error> bgeError;
            std::shared_ptr<BGETexture> texture;
            
            texture = std::dynamic_pointer_cast<BGETexture>(atlas);
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->texture_ = texture;
                this->textureName_ = atlasTextureKey();
                
                if (subTextures.size() > 0) {
                    for (auto& kv : subTextures) {
                        std::string key = kv.first;
                        subTex = BGEGame::getInstance()->getTextureService()->namedSubTexture(kv.first, a, kv.second.x, kv.second.y, kv.second.width, kv.second.height);
                        
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
                    bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorInvalidSubTexture);
                }
            } else {
                a.reset();
                bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorExistingTextureWrongType);
            }
            
            
            if (callback) {
                callback(a, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}
