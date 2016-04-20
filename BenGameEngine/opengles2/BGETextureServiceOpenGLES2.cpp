//
//  BGETextureServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETextureServiceOpenGLES2.h"
#include "BGETextureOpenGLES2.h"
#include "BGETextureAtlasOpenGLES2.h"

BGETextureServiceOpenGLES2::BGETextureServiceOpenGLES2(EAGLContext *context) : BGETextureService() {
    textureLoader_ = [[GLKTextureLoader alloc] initWithSharegroup:context.sharegroup];
}

BGETextureServiceOpenGLES2::~BGETextureServiceOpenGLES2() {
    
}

void BGETextureServiceOpenGLES2::initialize() {}
void BGETextureServiceOpenGLES2::reset() {}
void BGETextureServiceOpenGLES2::enteringBackground() {}
void BGETextureServiceOpenGLES2::enteringForeground() {}
void BGETextureServiceOpenGLES2::pause() {}
void BGETextureServiceOpenGLES2::resume() {}
void BGETextureServiceOpenGLES2::destroy() {}

void BGETextureServiceOpenGLES2::namedTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error>)> callback)
{
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator texIt = sTextures_.find(name);
    
    if (texIt == sTextures_.end()) {
        [textureLoader_ textureWithContentsOfFile:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding] options:@{ GLKTextureLoaderApplyPremultiplication: @(YES) } queue:nil completionHandler:^(GLKTextureInfo *textureInfo, NSError *error) {
            std::shared_ptr<BGE::Error> bgeError;
            std::shared_ptr<BGETextureOpenGLES2> oglTexture;
            
            if (textureInfo) {
                uint64_t texId = getIdAndIncrement();
                
                oglTexture = std::make_shared<BGETextureOpenGLES2>(texId, name, textureInfo);
                
                if (oglTexture) {
                    sTextures_[name] = oglTexture;
                    iTextures_[texId] = oglTexture;
                } else {
                    std::string domain = [error.domain UTF8String];
                    int32_t code = (uint32_t) error.code;
                    std::shared_ptr<BGE::Error> underlying = std::make_shared<BGE::Error>(domain, code);
                    bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorOS, underlying);
                }
            } else {
                std::string domain = [error.domain UTF8String];
                int32_t code = (uint32_t) error.code;
                std::shared_ptr<BGE::Error> underlying = std::make_shared<BGE::Error>(domain, code);
                bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorOS, underlying);
            }
            
            if (callback) {
                callback(oglTexture, bgeError);
            }
        }];
    } else if (callback) {
        callback(texIt->second, nullptr);
    }
}

void BGETextureServiceOpenGLES2::namedTextureFromURL(std::string name, std::string url, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error> error)> callback)
{
    
}

void BGETextureServiceOpenGLES2::namedTextureFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGE::Error>)> callback)
{
    // TODO: Add domains
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<BGE::Error> bgeError;
    
    if (texIt == sTextures_.end()) {
        std::shared_ptr<BGETextureOpenGLES2> oglTexture;
        uint64_t texId = getIdAndIncrement();
        
        oglTexture = std::make_shared<BGETextureOpenGLES2>(texId, name);
        
        if (oglTexture) {
            oglTexture->createFromBuffer(buffer, format, width, height, [this, &name, &callback, texId](std::shared_ptr<BGETexture> newTexture, std::shared_ptr<BGE::Error> error) -> void {
                
                if (newTexture) {
                    this->sTextures_[name] = newTexture;
                    this->iTextures_[texId] = newTexture;
                }
                
                if (callback) {
                    callback(newTexture, error);
                }
            });
        } else {
            bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorClassAllocation);
            
            if (callback) {
                callback(oglTexture, bgeError);
            }
        }
    } else if (callback) {
        callback(texIt->second, bgeError);
    }
}

void BGETextureServiceOpenGLES2::namedTextureAtlasFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGE::Error>)> callback) {
    // TODO: Add domains
    
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<BGE::Error> bgeError;
    
    if (texIt == sTextures_.end()) {
        std::shared_ptr<BGETextureAtlasOpenGLES2> oglTexture;
        uint64_t texId = getIdAndIncrement();

        oglTexture = std::make_shared<BGETextureAtlasOpenGLES2>(texId, name);
        
        if (oglTexture) {
            oglTexture->createFromBuffer(buffer, format, width, height, subTextureDefs, [this, &name, &callback, texId](std::shared_ptr<BGETextureAtlas> atlas, std::shared_ptr<BGE::Error> error) -> void {
                if (atlas) {
                    this->sTextures_[name] = atlas;
                    this->iTextures_[texId] = atlas;
                }
                
                if (callback) {
                    callback(atlas, error);
                }
            });
        } else {
            bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorClassAllocation);

            if (callback) {
                callback(oglTexture, bgeError);
            }
        }
    } else {
        std::shared_ptr<BGETextureAtlasOpenGLES2> texture;
        
        texture = std::dynamic_pointer_cast<BGETextureAtlasOpenGLES2>(texIt->second);
        
        assert(texture);
        
        if (!texture) {
            bgeError = std::make_shared<BGE::Error>(BGETextureBase::ErrorDomain, BGETextureErrorExistingTextureWrongType);
        }
        
        if (callback) {
            callback(texture, bgeError);
        }
    }
}


std::shared_ptr<BGETexture> BGETextureServiceOpenGLES2::namedSubTexture(std::string name, std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    // TODO: Add domains
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator texIt = sTextures_.find(name);
    std::shared_ptr<BGETextureOpenGLES2> texture;
    std::shared_ptr<BGE::Error> error;
    
    if (texIt == sTextures_.end()) {
        uint64_t texId = getIdAndIncrement();

        texture = std::make_shared<BGETextureOpenGLES2>(texId, name);
        
        if (texture) {
            error = texture->createSubTexture(atlas, x, y, width, height);
            
            if (!error) {
                sTextures_[name] = texture;
                iTextures_[texId] = texture;
            } else {
                texture.reset();
            }
        }
    } else {
        texture = std::dynamic_pointer_cast<BGETextureOpenGLES2>(sTextures_[name]);
    }
    
    return texture;
}

