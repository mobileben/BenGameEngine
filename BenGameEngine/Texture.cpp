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
#include "RawTexture.h"

#include <future>

#define RENDER_QUEUE

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

void BGE::Texture::initialize(TextureHandle handle, std::string name, TextureFormat format) {
    handle_ = handle;
    setName(name);

    valid_ = false;
    format_ = format;
    alphaState_ = TextureAlphaState::None;
    x_ = 0;
    y_ = 0;
    width_ = 0;
    height_ = 0;
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;

    atlasHandle_ = TextureAtlasHandle();
    isSubTexture_ = false;

    memoryUsage_ = computeMemoryUsage(format_, width_, height_);
}

void BGE::Texture::initialize(TextureHandle handle, std::string name, TextureFormat format, GLKTextureInfo *texInfo) {
    initialize(handle, name, format);
    
    if (texInfo) {
        valid_ = true;
        hwId_ = texInfo.name;
        target_ = texInfo.target;
        width_ = texInfo.width;
        height_ = texInfo.height;
        
        updateUVs();
        updateXYs();
        
        memoryUsage_ = computeMemoryUsage(format_, width_, height_);
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::initialize(TextureHandle handle, std::string name, RawTexture *rawTexture) {
    TextureFormat format = TextureFormat::Undefined;
    GLint glFormat = 0;
    
    switch (rawTexture->getFormat()) {
        case RawTexture::Format::RGB8_A8:
            format = TextureFormat::RGBA8888;
            glFormat = GL_RGBA;
            break;
            
        case RawTexture::Format::RGB8:
            format = TextureFormat::RGB888;
            glFormat = GL_RGB;
            break;
            
        default:
            assert(false);
            break;
    }
    
    initialize(handle, name, format);

    // Always delete textures on main thread (for now)
    auto width = rawTexture->getWidth();
    auto height = rawTexture->getHeight();
    auto buffer = rawTexture->getBuffer();

#ifdef RENDER_QUEUE
    auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
    auto fut = prom->get_future();
    Game::getInstance()->getRenderService()->queueCreateTexture(getHandle(), getFormat(), buffer, width, height, glFormat, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
        // Do the least amount of work inside
        if (!error) {
            auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
            valid_ = true;
            hwId_ = data->glHwId;
        } else {
            valid_ = false;
        }
        prom->set_value(error);
    });
    auto error = fut.get();

    if (!error) {
        format_ = format;
        target_ = GL_TEXTURE_2D;
        width_ = width;
        height_ = height;

        updateUVs();
        updateXYs();

        memoryUsage_ = computeMemoryUsage(format_, width_, height_);
    }

    return error;
#else
    __block std::shared_ptr<Error> error;
    auto block = ^{
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, buffer);
        GLenum glErr = glGetError();

        if (glErr == GL_NO_ERROR) {
            valid_ = true;
            hwId_ = tex;
            format_ = format;
            target_ = GL_TEXTURE_2D;
            width_ = width;
            height_ = height;

            updateUVs();
            updateXYs();

            memoryUsage_ = computeMemoryUsage(format_, width_, height_);
        } else {
            valid_ = false;
            error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
        }
    };
    if ([[NSThread currentThread] isMainThread]) {
        block();
    } else {
        dispatch_sync(dispatch_get_main_queue(), block);
    }
    return error;
#endif
}

void BGE::Texture::destroy() {
    valid_ = false;
    
    if (!isSubTexture_) {
        // Only non-subtextures can be freed, since the atlas is freed separately
        GLuint name;

        if (textureInfo_) {
            name = textureInfo_.name;
        } else {
            name = hwId_;
        }
        
        // Always delete textures on main thread (for now)
#ifdef RENDER_QUEUE
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
        Game::getInstance()->getRenderService()->queueDestroyTexture(getHandle(), name, [prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            prom->set_value(error);
        });
        fut.get();
#else
        auto block = ^{
            glDeleteTextures(1, &name);
        };
        if ([[NSThread currentThread] isMainThread]) {
            block();
        } else {
            dispatch_sync(dispatch_get_main_queue(), block);
        }
#endif
    }

    hwId_ = 0;
    textureInfo_ = nil;
    handle_ = TextureHandle();
    atlasHandle_ = TextureAtlasHandle();
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

std::shared_ptr<BGE::Error> BGE::Texture::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height) {
    std::shared_ptr<Error> error = std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
    switch (format) {
        case TextureFormat::Alpha:
            error = createTextureFromAlphaBuffer((unsigned char *) buffer, width, height);
            break;
            
        case TextureFormat::RGB565:
            error = createTextureFromRGB565Buffer((unsigned char *) buffer, width, height);
            break;
            
        case TextureFormat::RGB888:
            error = createTextureFromRGB888Buffer((unsigned char *) buffer, width, height);
            break;
            
        case TextureFormat::RGBA5551:
            error = createTextureFromRGBA5551Buffer((unsigned char *) buffer, width, height);
            break;
            
        case TextureFormat::RGBA4444:
            error = createTextureFromRGBA4444Buffer((unsigned char *) buffer, width, height);
            break;
            
        case TextureFormat::RGBA8888:
            error = createTextureFromRGBA8888Buffer((unsigned char *) buffer, width, height);
            break;
            
        default:
            assert(false);  // Unsupported
            break;
    }

    if (!error) {
        this->setValid(true);
        this->setFormat(format);
        this->setWidth(width);
        this->setHeight(height);

        this->updateUVs();
        this->updateXYs();

        // Compute texture memory usage
        memoryUsage_ = computeMemoryUsage(format, width, height);
    }
    return error;
}

std::shared_ptr<BGE::Error> BGE::Texture::createSubTexture(TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated) {
    std::shared_ptr<Error> error;

    // Must tag as sub-texture up front so we don't falsely release texture
    isSubTexture_ = true;

    if (atlas && width != 0 && height != 0) {
        if (atlas) {
            atlasHandle_ = atlas->getHandle();
            valid_ = true;
            format_ = atlas->getFormat();
            alphaState_ = atlas->getAlphaState();
            
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
            
            // Memory usage is considered 0 since the underlying texture of the atlas tracks memory usage
            memoryUsage_ = 0;
        }
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorInvalidSubTexture);
    }
    
    return error;
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    if (buffer) {
#ifdef RENDER_QUEUE
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
        Game::getInstance()->getRenderService()->queueCreateTexture(getHandle(), getFormat(), buffer, width, height, GL_ALPHA, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
                hwId_ = data->glHwId;
            }
            prom->set_value(error);
        });
        return fut.get();
#else
        __block std::shared_ptr<Error> error;
        auto __block block = ^() {
            GLuint tex;

            glGenTextures(1, &tex);

            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer);

            GLenum glErr = glGetError();
            if (glErr == GL_NO_ERROR) {
                valid_ = true;
                hwId_ = tex;
            } else {
                error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
            }
        };
        if ([[NSThread currentThread] isMainThread]) {
            block();
        } else {
            dispatch_sync(dispatch_get_main_queue(), block);
        }

        return error;
#endif
    } else {
        return std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    if (buffer) {
#ifdef RENDER_QUEUE
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
        Game::getInstance()->getRenderService()->queueCreateTexture(getHandle(), getFormat(), buffer, width, height, GL_RGB, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
                hwId_ = data->glHwId;
            }
            prom->set_value(error);
        });
        return fut.get();
#else
        __block std::shared_ptr<Error> error;
        auto block = ^() {
            GLuint tex;

            glGenTextures(1, &tex);
            GLint alignment;
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
            glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

            GLenum glErr = glGetError();

            if (glErr == GL_NO_ERROR) {
                valid_ = true;
                hwId_ = tex;
            } else {
                error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
            }
        };
        if ([[NSThread currentThread] isMainThread]) {
            block();
        } else {
            dispatch_sync(dispatch_get_main_queue(), block);
        }
        return error;
#endif
    } else {
        return std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    if (buffer) {
#ifdef RENDER_QUEUE
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
        Game::getInstance()->getRenderService()->queueCreateTexture(getHandle(), getFormat(), buffer, width, height, GL_RGBA, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
                hwId_ = data->glHwId;
            }
            prom->set_value(error);
        });
        return fut.get();
#else
        __block std::shared_ptr<Error> error;
        auto block = ^() {
            GLuint tex;

            glGenTextures(1, &tex);

            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

            GLenum glErr = glGetError();

            if (glErr == GL_NO_ERROR) {
                valid_ = true;
                hwId_ = tex;
            } else {
                error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
            }
        };
        if ([[NSThread currentThread] isMainThread]) {
            block();
        } else {
            dispatch_sync(dispatch_get_main_queue(), block);
        }
        return error;
#endif
    } else {
        return std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
}

size_t BGE::Texture::computeMemoryUsage(TextureFormat format, uint32_t width, uint32_t height) {
    switch (format) {
        case TextureFormat::Alpha:
            return width * height;
            
        case TextureFormat::RGB565:
        case TextureFormat::RGBA5551:
        case TextureFormat::RGBA4444:
            return width * height * 2;
            
        case TextureFormat::RGB888:
            return width * height * 3;
            
        case TextureFormat::RGBA8888:
            return width * height * 4;
            
        default:
            assert(false);
    }
    
    return 0;
}

