//
//  BGETextureOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGETextureOpenGLES2.h"
#include "BGETextureAtlas.h"
#include "BGETextureAtlasOpenGLES2.h"

BGETextureOpenGLES2::BGETextureOpenGLES2(std::string name) : BGETexture(name), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(nil) {
}

BGETextureOpenGLES2::BGETextureOpenGLES2(std::string name, GLKTextureInfo *textureInfo) : BGETexture(name), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(textureInfo) {
    format_ = BGETextureFormat::RGBA8888;
    
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

BGETextureOpenGLES2::~BGETextureOpenGLES2() {
    GLuint name;
    
    if (textureInfo_) {
        name = textureInfo_.name;
    } else {
        name = hwId_;
    }
    
    glDeleteTextures(1, &name);
}

uint32_t BGETextureOpenGLES2::getHWTextureId() const {
    if (textureInfo_) {
        return textureInfo_.name;
    } else {
        return hwId_;
    }
}

GLenum BGETextureOpenGLES2::getTarget() const {
    if (textureInfo_) {
        return textureInfo_.target;
    } else {
        return target_;
    }
}

void BGETextureOpenGLES2::createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETexture>, std::shared_ptr<BGEError>)> callback) {
    std::function<void(std::shared_ptr<BGEError>)> handler = [this, format, width, height, &callback](std::shared_ptr<BGEError> error) -> void {
        std::shared_ptr<BGETexture> texture;
        
        if (!error) {
            texture = std::dynamic_pointer_cast<BGETexture>(shared_from_this());
            
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
    
    releaseCurrentTexture();
    
    switch (format) {
        case BGETextureFormat::Alpha:
            createTextureFromAlphaBuffer((unsigned char *) buffer, width, height, handler);
            break;
            
        case BGETextureFormat::RGB565:
            break;
            
        case BGETextureFormat::RGB888:
            break;
            
        case BGETextureFormat::RGBA5551:
            break;
            
        case BGETextureFormat::RGBA4444:
            break;
            
        case BGETextureFormat::RGBA8888:
            break;
            
        default:
            break;
    }
}

std::shared_ptr<BGEError> BGETextureOpenGLES2::createSubTexture(std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    releaseCurrentTexture();
    
    std::shared_ptr<BGEError> error;
    
    if (atlas && width != 0 && height != 0) {
        std::shared_ptr<BGETextureAtlasOpenGLES2> oglAtlas = std::dynamic_pointer_cast<BGETextureAtlasOpenGLES2>(atlas);
        
        if (oglAtlas) {
            atlas_ = atlas;
            valid_ = true;
            format_ = oglAtlas->getFormat();
            alphaState_ = oglAtlas->getAlphaState();
            isSubTexture_ = true;
            
            x_ = x;
            y_ = y;
            width_ = width;
            height_ = height;
            
            hwId_ = oglAtlas->getHWTextureId();
            target_ = oglAtlas->getTarget();
            
            updateUVs();
            updateXYs();
            
            NSLog(@"Created Subtexture %s", name_.c_str());
        }
    } else {
        error = std::make_shared<BGEError>(BGETextureBase::ErrorDomain, BGETextureErrorInvalidSubTexture);
    }
    
    return error;
}

void BGETextureOpenGLES2::createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;

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
            error = std::make_shared<BGEError>(BGETextureBase::ErrorDomain, BGETextureErrorAllocation);
        }
    } else {
        error = std::make_shared<BGEError>(BGETextureBase::ErrorDomain, BGETextureErrorNoBuffer);
    }
    
    if (callback) {
        callback(error);
    }
}

void BGETextureOpenGLES2::createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;
    
    if (callback) {
        callback(error);
    }
}

void BGETextureOpenGLES2::createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;
    
    if (callback) {
        callback(error);
    }
}

void BGETextureOpenGLES2::createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;
    
    if (callback) {
        callback(error);
    }
}

void BGETextureOpenGLES2::createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;

    if (callback) {
        callback(error);
    }
}

void BGETextureOpenGLES2::createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback) {
    std::shared_ptr<BGEError> error;

    if (callback) {
        callback(error);
    }
}

