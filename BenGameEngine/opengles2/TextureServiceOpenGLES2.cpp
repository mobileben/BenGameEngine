//
//  TextureServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureServiceOpenGLES2.h"
#include "TextureOpenGLES2.h"
#include "TextureAtlasOpenGLES2.h"

BGE::TextureServiceOpenGLES2::TextureServiceOpenGLES2(EAGLContext *context) : TextureService() {
    textureLoader_ = [[GLKTextureLoader alloc] initWithSharegroup:context.sharegroup];
}

BGE::TextureServiceOpenGLES2::~TextureServiceOpenGLES2() {
    
}

void BGE::TextureServiceOpenGLES2::initialize() {}
void BGE::TextureServiceOpenGLES2::reset() {}
void BGE::TextureServiceOpenGLES2::enteringBackground() {}
void BGE::TextureServiceOpenGLES2::enteringForeground() {}
void BGE::TextureServiceOpenGLES2::pause() {}
void BGE::TextureServiceOpenGLES2::resume() {}
void BGE::TextureServiceOpenGLES2::destroy() {}

void BGE::TextureServiceOpenGLES2::createTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback)
{
    std::unordered_map<std::string, std::shared_ptr<TextureBase>>::iterator texIt = sTextures_.find(name);
    
    if (texIt == sTextures_.end()) {
        [textureLoader_ textureWithContentsOfFile:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding] options:@{ GLKTextureLoaderApplyPremultiplication: @(YES) } queue:nil completionHandler:^(GLKTextureInfo *textureInfo, NSError *error) {
            std::shared_ptr<Error> bgeError;
            std::shared_ptr<TextureOpenGLES2> oglTexture;
            
            if (textureInfo) {
                uint32_t texId = getIdAndIncrement();
                
                oglTexture = std::make_shared<TextureOpenGLES2>(texId, name, textureInfo);
                
                if (oglTexture) {
                    sTextures_[name] = oglTexture;
                    iTextures_[texId] = oglTexture;
                } else {
                    std::string domain = [error.domain UTF8String];
                    int32_t code = (uint32_t) error.code;
                    std::shared_ptr<Error> underlying = std::make_shared<Error>(domain, code);
                    bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorOS, underlying);
                }
            } else {
                std::string domain = [error.domain UTF8String];
                int32_t code = (uint32_t) error.code;
                std::shared_ptr<Error> underlying = std::make_shared<BGE::Error>(domain, code);
                bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorOS, underlying);
            }
            
            if (callback) {
                callback(oglTexture, bgeError);
            }
        }];
    } else if (callback) {
        callback(texIt->second, nullptr);
    }
}

void BGE::TextureServiceOpenGLES2::createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error> error)> callback) {
    std::unordered_map<std::string, std::shared_ptr<TextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<Error> bgeError;

    if (texIt == sTextures_.end()) {
        std::shared_ptr<TextureAtlasOpenGLES2> oglTexture;
        uint32_t texId = getIdAndIncrement();
        
        oglTexture = std::make_shared<TextureAtlasOpenGLES2>(texId, name);
        
        if (oglTexture) {
            oglTexture->createFromFile(filename, subTextureDefs, [this, oglTexture, name, callback, texId](std::shared_ptr<TextureAtlas> atlas, std::shared_ptr<Error> error) -> void {
                if (atlas) {
                    this->sTextures_[name] = atlas;
                    this->iTextures_[texId] = atlas;
                }
                
                if (callback) {
                    callback(atlas, error);
                }
            });
        } else {
            bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorClassAllocation);
            
            if (callback) {
                callback(oglTexture, bgeError);
            }
        }
    } else {
        std::shared_ptr<TextureAtlasOpenGLES2> texture;
        
        texture = std::dynamic_pointer_cast<TextureAtlasOpenGLES2>(texIt->second);
        
        assert(texture);
        
        if (!texture) {
            bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorExistingTextureWrongType);
        }
        
        if (callback) {
            callback(texture, bgeError);
        }
    }
}

void BGE::TextureServiceOpenGLES2::createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback)
{
    // TODO: Add domains
    std::unordered_map<std::string, std::shared_ptr<TextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<Error> bgeError;
    
    if (texIt == sTextures_.end()) {
        std::shared_ptr<TextureOpenGLES2> oglTexture;
        uint32_t texId = getIdAndIncrement();
        
        oglTexture = std::make_shared<TextureOpenGLES2>(texId, name);
        
        if (oglTexture) {
            oglTexture->createFromBuffer(buffer, format, width, height, [this, &name, &callback, texId](std::shared_ptr<Texture> newTexture, std::shared_ptr<Error> error) -> void {
                
                if (newTexture) {
                    this->sTextures_[name] = newTexture;
                    this->iTextures_[texId] = newTexture;
                }
                
                if (callback) {
                    callback(newTexture, error);
                }
            });
        } else {
            bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorClassAllocation);
            
            if (callback) {
                callback(oglTexture, bgeError);
            }
        }
    } else if (callback) {
        callback(texIt->second, bgeError);
    }
}

void BGE::TextureServiceOpenGLES2::createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback) {
    // TODO: Add domains
    
    std::unordered_map<std::string, std::shared_ptr<TextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<Error> bgeError;
    
    if (texIt == sTextures_.end()) {
        std::shared_ptr<TextureAtlasOpenGLES2> oglTexture;
        uint32_t texId = getIdAndIncrement();

        oglTexture = std::make_shared<TextureAtlasOpenGLES2>(texId, name);
        
        if (oglTexture) {
            oglTexture->createFromBuffer(buffer, format, width, height, subTextureDefs, [this, &name, &callback, texId](std::shared_ptr<TextureAtlas> atlas, std::shared_ptr<Error> error) -> void {
                if (atlas) {
                    this->sTextures_[name] = atlas;
                    this->iTextures_[texId] = atlas;
                }
                
                if (callback) {
                    callback(atlas, error);
                }
            });
        } else {
            bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorClassAllocation);

            if (callback) {
                callback(oglTexture, bgeError);
            }
        }
    } else {
        std::shared_ptr<TextureAtlasOpenGLES2> texture;
        
        texture = std::dynamic_pointer_cast<TextureAtlasOpenGLES2>(texIt->second);
        
        assert(texture);
        
        if (!texture) {
            bgeError = std::make_shared<Error>(TextureBase::ErrorDomain, TextureErrorExistingTextureWrongType);
        }
        
        if (callback) {
            callback(texture, bgeError);
        }
    }
}


std::shared_ptr<BGE::Texture> BGE::TextureServiceOpenGLES2::createSubTexture(std::string name, std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated) {
    // TODO: Add domains
    std::unordered_map<std::string, std::shared_ptr<TextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<TextureOpenGLES2> texture;
    std::shared_ptr<Error> error;
    
    if (texIt == sTextures_.end()) {
        uint32_t texId = getIdAndIncrement();

        texture = std::make_shared<TextureOpenGLES2>(texId, name);
        
        if (texture) {
            error = texture->createSubTexture(atlas, x, y, width, height, rotated);
            
            if (!error) {
                sTextures_[name] = texture;
                iTextures_[texId] = texture;
            } else {
                texture.reset();
            }
        }
    } else {
        texture = std::dynamic_pointer_cast<TextureOpenGLES2>(sTextures_[name]);
    }
    
    return texture;
}

