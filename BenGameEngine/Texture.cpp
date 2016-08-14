//
//  Texture.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Texture.h"
#include "TextureAtlas.h"
#include "Game.h"

const std::string BGE::Texture::ErrorDomain = "Texture";

BGE::Texture::Texture() : NamedObject(), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), x_(0), y_(0), width_(0), height_(0), isSubTexture_(false), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(nil) {
    updateUVs();
    updateXYs();
}

#ifdef NOT_YET
BGE::Texture::Texture(uint32_t texId) : NamedObject(texId), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), x_(0), y_(0), width_(0), height_(0), isSubTexture_(false), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(nil) {
    updateUVs();
    updateXYs();
}

BGE::Texture::Texture(uint32_t texId, std::string name) : NamedObject(texId, name), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), x_(0), y_(0), width_(0), height_(0), isSubTexture_(false), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(nil) {
    updateUVs();
    updateXYs();
}

BGE::Texture::Texture(uint32_t texId, std::string name, GLKTextureInfo *textureInfo) : NamedObject(texId, name), valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), x_(0), y_(0), width_(0), height_(0), isSubTexture_(false), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(textureInfo) {
    format_ = TextureFormat::RGBA8888;
    
    if (textureInfo) {
        valid_ = true;
        hwId_ = textureInfo.name;
        target_ = textureInfo.target;
        width_ = textureInfo.width;
        height_ = textureInfo.height;
        
        updateUVs();
        updateXYs();
    }
}
#endif

BGE::Texture::~Texture() {
    GLuint name;
    
    if (textureInfo_) {
        name = textureInfo_.name;
    } else {
        name = hwId_;
    }
    
    glDeleteTextures(1, &name);
}

void BGE::Texture::initialize(TextureHandle handle, ObjectId texId, std::string name) {
    handle_ = handle;
    setInstanceId(texId);
    setName(name);

    valid_ = false;
    format_ = TextureFormat::Undefined;
    alphaState_ = TextureAlphaState::None;
    x_ = 0;
    y_ = 0;
    width_ = 0;
    height_ = 0;
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;

    atlasHandle_ = TextureAtlasHandle();
    isSubTexture_ = false;
}

void BGE::Texture::initialize(TextureHandle handle, ObjectId texId, std::string name, GLKTextureInfo *texInfo) {
    initialize(handle, texId, name);
    
    if (texInfo) {
        // TODO: Can we ascertain the right format?
        format_ = TextureFormat::RGBA8888;

        valid_ = true;
        hwId_ = texInfo.name;
        target_ = texInfo.target;
        width_ = texInfo.width;
        height_ = texInfo.height;
        
        updateUVs();
        updateXYs();
    }
}

void BGE::Texture::updateUVs(bool rotated) {
    if (!isSubTexture_) {
        /*
         Defaults to OpenGL setup
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         */
        
        if (rotated) {
            uvs_[3].x = 0;
            uvs_[3].y = 0;
            uvs_[0].x = 1;
            uvs_[0].y = 0;
            uvs_[1].x = 1;
            uvs_[1].y = 1;
            uvs_[2].x = 0;
            uvs_[2].y = 1;
        } else {
            uvs_[0].x = 0;
            uvs_[0].y = 0;
            uvs_[1].x = 1;
            uvs_[1].y = 0;
            uvs_[2].x = 1;
            uvs_[2].y = 1;
            uvs_[3].x = 0;
            uvs_[3].y = 1;
        }
    } else {
        auto atlas = Game::getInstance()->getTextureService()->getTextureAtlas(atlasHandle_);
        
        if (atlas) {
            float x = x_;
            float y = y_;
            float width = width_;
            float height = height_;
            float atlasW = atlas->getWidth();
            float atlasH = atlas->getHeight();
            
            /*
             Defaults to OpenGL setup
             
             0 ------- 1
             | \      |
             |  \     |
             |   \    |
             |    \   |
             |     \  |
             |      \ |
             |       \|
             3--------2
             
             */
            if (rotated) {
                // If rotated, then width and height have been swapped, so swap them
                width = height_;
                height = width_;
                
                uvs_[3].x = x / atlasW;
                uvs_[3].y = y / atlasH;
                uvs_[0].x = (x + width) / atlasW;
                uvs_[0].y = y / atlasH;
                uvs_[1].x = (x + width) / atlasW;
                uvs_[1].y = (y + height) / atlasH;
                uvs_[2].x = x / atlasW;
                uvs_[2].y = (y + height) / atlasH;
            } else {
                uvs_[0].x = x / atlasW;
                uvs_[0].y = y / atlasH;
                uvs_[1].x = (x + width) / atlasW;
                uvs_[1].y = y / atlasH;
                uvs_[2].x = (x + width) / atlasW;
                uvs_[2].y = (y + height) / atlasH;
                uvs_[3].x = x / atlasW;
                uvs_[3].y = (y + height) / atlasH;
            }
        }
    }
}

void BGE::Texture::updateXYs() {
    if (Game::getInstance()->getRenderService()->hasInvertedYAxis()) {
        /*
         
         +Y down
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 1 - 2
         0 - 2 - 3
         */
        
        xys_[0].x = 0;
        xys_[0].y = getHeight();
        
        xys_[1].x = getWidth();
        xys_[1].y = getHeight();
        
        xys_[2].x = getWidth();
        xys_[2].y = 0;
        
        xys_[3].x = 0;
        xys_[3].y = 0;
    } else {
        /*
         
         +Y up
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 1 - 2
         0 - 2 - 3
         */
        
        xys_[0].x = 0;
        xys_[0].y = 0;
        
        xys_[1].x = getWidth();
        xys_[1].y = 0;
        
        xys_[2].x = getWidth();
        xys_[2].y = getHeight();
        
        xys_[3].x = 0;
        xys_[3].y = getHeight();
    }
}

void BGE::Texture::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    std::function<void(std::shared_ptr<Error>)> handler = [this, format, width, height, &callback](std::shared_ptr<Error> error) -> void {
        Texture *texture = nullptr;
        
        if (!error) {
            texture = this;
            
            if (texture) {
                this->setValid(true);
                this->setFormat(format);
                this->setWidth(width);
                this->setHeight(height);
                
                this->updateUVs();
                this->updateXYs();
            }
        }
        
        if (callback) {
            callback(texture, error);
        }
    };
    
    switch (format) {
        case TextureFormat::Alpha:
            createTextureFromAlphaBuffer((unsigned char *) buffer, width, height, handler);
            break;
            
        case TextureFormat::RGB565:
            break;
            
        case TextureFormat::RGB888:
            break;
            
        case TextureFormat::RGBA5551:
            break;
            
        case TextureFormat::RGBA4444:
            break;
            
        case TextureFormat::RGBA8888:
            break;
            
        default:
            break;
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::createSubTexture(TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated) {
    std::shared_ptr<Error> error;
    
    if (atlas && width != 0 && height != 0) {
        if (atlas) {
            atlasHandle_ = atlas->getHandle();
            valid_ = true;
            format_ = atlas->getFormat();
            alphaState_ = atlas->getAlphaState();
            isSubTexture_ = true;
            
            x_ = x;
            y_ = y;
            
            if (rotated) {
                width_ = height;
                height_ = width;
            } else {
                width_ = width;
                height_ = height;
            }
            
            hwId_ = atlas->getHWTextureId();
            target_ = atlas->getTarget();
            
            updateUVs(rotated);
            updateXYs();
            
            NSLog(@"Created Subtexture %s", getName().c_str());
        }
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorInvalidSubTexture);
    }
    
    return error;
}

void BGE::Texture::createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (buffer) {
        GLuint tex;
        
        glGenTextures(1, &tex);
        
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer);
        
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        GLenum glErr = glGetError();
        
        if (glErr == GL_NO_ERROR) {
            valid_ = true;
            hwId_ = tex;
        } else {
            error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
        }
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
    
    if (callback) {
        callback(error);
    }
}

void BGE::Texture::createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::Texture::createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::Texture::createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::Texture::createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::Texture::createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> error;
    
    if (callback) {
        callback(error);
    }
}

