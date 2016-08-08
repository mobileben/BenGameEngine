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

BGE::TextureAtlasOpenGLES2::TextureAtlasOpenGLES2(uint32_t texId, std::string name) : TextureAtlas(texId ,name) {
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
    std::shared_ptr<TextureOpenGLES2> oglTexture = std::dynamic_pointer_cast<TextureOpenGLES2>(texture_);
    
    if (oglTexture) {
        return oglTexture->getTarget();
    } else {
        return 0;
    }
}

void BGE::TextureAtlasOpenGLES2::createFromFile(std::string filename, std::vector<SubTextureDef> &subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback) {
    releaseCurrentTexture();
    
    Game::getInstance()->getTextureService()->namedTextureFromFile(atlasTextureKey(), filename, [this, &subTextures, callback](std::shared_ptr<TextureBase> atlas, std::shared_ptr<Error> error) {
        if (!error) {
            std::shared_ptr<TextureAtlas> a = derived_shared_from_this<TextureAtlas>();
            std::shared_ptr<Texture> subTex;
            std::shared_ptr<Error> bgeError;
            std::shared_ptr<Texture> texture;
            
            texture = std::dynamic_pointer_cast<Texture>(atlas);
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->texture_ = texture;
                this->textureName_ = atlasTextureKey();
                
                if (subTextures.size() > 0) {
                    for (auto& st : subTextures) {
                        std::string key = st.name;
                        subTex = Game::getInstance()->getTextureService()->namedSubTexture(key, a, st.x, st.y, st.width, st.height, st.rotated);
                        
                        if (subTex) {
                            this->subTextures_[key] = subTex;
                        }
                    }
                    
                    this->valid_ = true;
                    this->format_ = texture->getFormat();
                    this->alphaState_ = texture->getAlphaState();
                    this->width_ = texture->getWidth();
                    this->height_ = texture->getHeight();
                } else {
                    a.reset();
                    bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorInvalidSubTexture);
                }
            } else {
                a.reset();
                bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorExistingTextureWrongType);
            }
            
            
            if (callback) {
                callback(a, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}

void BGE::TextureAtlasOpenGLES2::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback) {
    releaseCurrentTexture();

    Game::getInstance()->getTextureService()->namedTextureFromBuffer(atlasTextureKey(), buffer, format, width, height, [this, &subTextures, &callback](std::shared_ptr<TextureBase> atlas, std::shared_ptr<Error> error) {
        if (!error) {
            std::shared_ptr<TextureAtlas> a = derived_shared_from_this<TextureAtlas>();
            std::shared_ptr<Texture> subTex;
            std::shared_ptr<Error> bgeError;
            std::shared_ptr<Texture> texture;
            
            texture = std::dynamic_pointer_cast<Texture>(atlas);
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->texture_ = texture;
                this->textureName_ = atlasTextureKey();
                
                if (subTextures.size() > 0) {
                    for (auto& st : subTextures) {
                        std::string key = st.name;
                        subTex = Game::getInstance()->getTextureService()->namedSubTexture(key, a, st.x, st.y, st.width, st.height, st.rotated);
                        
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
                    bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorInvalidSubTexture);
                }
            } else {
                a.reset();
                bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorExistingTextureWrongType);
            }
            
            
            if (callback) {
                callback(a, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}
