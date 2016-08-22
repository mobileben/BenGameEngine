//
//  TextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureAtlas.h"
#include "Game.h"

const std::string BGE::TextureAtlas::ErrorDomain = "TextureAtlas";

BGE::TextureAtlas::TextureAtlas() : NamedObject(), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), width_(0), height_(0), hwId_(0), target_(GL_TEXTURE_2D) {
}

BGE::TextureAtlas::TextureAtlas(ObjectId texId) : NamedObject(texId), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), width_(0), height_(0), hwId_(0), target_(GL_TEXTURE_2D) {
}

BGE::TextureAtlas::TextureAtlas(ObjectId texId, std::string name) : NamedObject(texId, name), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), width_(0), height_(0), hwId_(0), target_(GL_TEXTURE_2D) {
}

BGE::TextureAtlas::~TextureAtlas() {
}

void BGE::TextureAtlas::reset() {
    valid_ = false;
    handle_ = TextureAtlasHandle();
    textureHandle_ = TextureHandle();
    
    format_ = TextureFormat::Undefined;
    alphaState_ = TextureAlphaState::None;
    width_ = 0;
    height_ = 0;
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;
    
    subTextures_.clear();
}

void BGE::TextureAtlas::initialize(TextureAtlasHandle handle, std::string name) {
    reset();
    
    handle_ = handle;
    setName(name);
}

std::string BGE::TextureAtlas::atlasTextureKey() const {
    return "__" + getName() + "_texture";
}

BGE::TextureHandle BGE::TextureAtlas::getSubTextureHandle(std::string name) {
    auto subTex = subTextures_.find(name);
    
    if (subTex != subTextures_.end()) {
        return subTex->second;
    } else {
        return TextureHandle();
    }
}

void BGE::TextureAtlas::createFromFile(std::string filename, std::vector<SubTextureDef> &subTextures, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    Game::getInstance()->getTextureService()->createTextureFromFile(getHandle(), atlasTextureKey(), filename, [this, &subTextures, callback](Texture *texture, std::shared_ptr<Error> error) {
        if (!error) {
            std::shared_ptr<Error> bgeError;
            TextureAtlas *atlas = this;
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->textureHandle_ = texture->getHandle();

                this->valid_ = true;
                this->format_ = texture->getFormat();
                this->alphaState_ = texture->getAlphaState();
                this->width_ = texture->getWidth();
                this->height_ = texture->getHeight();
                this->hwId_ = texture->getHWTextureId();
                this->target_ = texture->getTarget();

                if (subTextures.size() > 0) {
                    for (auto& st : subTextures) {
                        std::string key = st.name;
                        auto subTex = Game::getInstance()->getTextureService()->createSubTexture(getHandle(), key, this, st.x, st.y, st.width, st.height, st.rotated);
                        
                        if (subTex) {
                            this->subTextures_[key] = subTex->getHandle();
                        }
                    }
                } else {
                    valid_ = false;
                    format_ = TextureFormat::Undefined;
                    alphaState_ = TextureAlphaState::None;
                    width_ = 0;
                    height_ = 0;
                    hwId_ = 0;
                    target_ = GL_TEXTURE_2D;

                    atlas = nullptr;
                    bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorInvalidSubTexture);
                }
            } else {
                atlas = nullptr;
                bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorExistingTextureWrongType);
            }
            
            if (callback) {
                callback(atlas, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}

void BGE::TextureAtlas::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextures, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    Game::getInstance()->getTextureService()->createTextureFromBuffer(getHandle(), atlasTextureKey(), buffer, format, width, height, [this, &subTextures, &callback](Texture *texture, std::shared_ptr<Error> error) {
        if (!error) {
            std::shared_ptr<Error> bgeError;
            TextureAtlas *atlas = this;
            
            if (texture) {
                // Texture needs to be set before processing subTex
                this->textureHandle_ = texture->getHandle();
                
                this->valid_ = true;
                this->format_ = texture->getFormat();
                this->alphaState_ = texture->getAlphaState();
                this->width_ = texture->getWidth();
                this->height_ = texture->getHeight();
                this->hwId_ = texture->getHWTextureId();
                this->target_ = texture->getTarget();
                
                if (subTextures.size() > 0) {
                    for (auto& st : subTextures) {
                        std::string key = st.name;
                        auto subTex = Game::getInstance()->getTextureService()->createSubTexture(getHandle(), key, this, st.x, st.y, st.width, st.height, st.rotated);
                        
                        if (subTex) {
                            subTextures_[key] = subTex->getHandle();
                        }
                    }
                } else {
                    valid_ = false;
                    format_ = TextureFormat::Undefined;
                    alphaState_ = TextureAlphaState::None;
                    width_ = 0;
                    height_ = 0;
                    hwId_ = 0;
                    target_ = GL_TEXTURE_2D;

                    atlas = nullptr;
                    bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorInvalidSubTexture);
                }
            } else {
                atlas = nullptr;
                bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorExistingTextureWrongType);
            }
            
            if (callback) {
                callback(atlas, bgeError);
            }
        } else if (callback) {
            callback(nullptr, error);
        }
    });
}