//
//  TextureOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureOpenGLES2.h"
#include "TextureAtlas.h"
#include "TextureAtlasOpenGLES2.h"

BGE::TextureOpenGLES2::TextureOpenGLES2(uint64_t texId, std::string name) : Texture(texId, name), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(nil) {
}

BGE::TextureOpenGLES2::TextureOpenGLES2(uint64_t texId, std::string name, GLKTextureInfo *textureInfo) : Texture(texId, name), hwId_(0), target_(GL_TEXTURE_2D), textureInfo_(textureInfo) {
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

BGE::TextureOpenGLES2::~TextureOpenGLES2() {
    GLuint name;
    
    if (textureInfo_) {
        name = textureInfo_.name;
    } else {
        name = hwId_;
    }
    
    glDeleteTextures(1, &name);
}

uint32_t BGE::TextureOpenGLES2::getHWTextureId() const {
    if (textureInfo_) {
        return textureInfo_.name;
    } else {
        return hwId_;
    }
}

GLenum BGE::TextureOpenGLES2::getTarget() const {
    if (textureInfo_) {
        return textureInfo_.target;
    } else {
        return target_;
    }
}

void BGE::TextureOpenGLES2::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Texture>, std::shared_ptr<BGE::Error>)> callback) {
    std::function<void(std::shared_ptr<BGE::Error>)> handler = [this, format, width, height, &callback](std::shared_ptr<BGE::Error> error) -> void {
        std::shared_ptr<Texture> texture;
        
        if (!error) {
            texture = std::dynamic_pointer_cast<Texture>(shared_from_this());
            
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

std::shared_ptr<BGE::Error> BGE::TextureOpenGLES2::createSubTexture(std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    releaseCurrentTexture();
    
    std::shared_ptr<BGE::Error> error;
    
    if (atlas && width != 0 && height != 0) {
        std::shared_ptr<TextureAtlasOpenGLES2> oglAtlas = std::dynamic_pointer_cast<TextureAtlasOpenGLES2>(atlas);
        
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
            
            NSLog(@"Created Subtexture %s", getName().c_str());
        }
    } else {
        error = std::make_shared<BGE::Error>(TextureBase::ErrorDomain, TextureErrorInvalidSubTexture);
    }
    
    return error;
}

void BGE::TextureOpenGLES2::createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;

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
            error = std::make_shared<BGE::Error>(BGE::TextureBase::ErrorDomain, TextureErrorAllocation);
        }
    } else {
        error = std::make_shared<BGE::Error>(TextureBase::ErrorDomain, TextureErrorNoBuffer);
    }
    
    if (callback) {
        callback(error);
    }
}

void BGE::TextureOpenGLES2::createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::TextureOpenGLES2::createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::TextureOpenGLES2::createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;
    
    if (callback) {
        callback(error);
    }
}

void BGE::TextureOpenGLES2::createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;

    if (callback) {
        callback(error);
    }
}

void BGE::TextureOpenGLES2::createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback) {
    std::shared_ptr<BGE::Error> error;

    if (callback) {
        callback(error);
    }
}

