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

const std::string BGE::Texture::ErrorDomain = "Texture";

BGE::Texture::Texture() : NamedObject(), valid_(false), width_(0), height_(0), x_(0), y_(0), isSubTexture_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None) {
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;
    vboId_ = 0;
    iboId_ = 0;
    iboOffset_ = 0;
#endif /* SUPPORT_OPENGL */
#ifdef SUPPORT_GLKTEXTURELOADER
    textureInfo_ = nil;
#endif /* SUPPORT_GLKTEXTURELOADER */
    updateUVs();
    updateXYs();
}

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
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;
    vboId_ = 0;
    iboId_ = 0;
    iboOffset_ = 0;
#endif /* SUPPORT_OPENGL */

    atlasHandle_ = TextureAtlasHandle();
    isSubTexture_ = false;

    memoryUsage_ = computeMemoryUsage(format_, width_, height_);
}

#ifdef SUPPORT_GLKTEXTURELOADER
void BGE::Texture::initialize(TextureHandle handle, std::string name, TextureFormat format, GLKTextureInfo *texInfo) {
    initialize(handle, name, format);
    
    if (texInfo) {
        valid_ = true;
#ifdef SUPPORT_OPENGL
        hwId_ = texInfo.name;
        target_ = texInfo.target;
        vboId_ = 0;
        iboId_ = 0;
        iboOffset_ = 0;
#endif /* SUPPORT_OPENGL */
        width_ = texInfo.width;
        height_ = texInfo.height;
        
        updateUVs();
        updateXYs();
        
        memoryUsage_ = computeMemoryUsage(format_, width_, height_);
    }
}
#endif /* SUPPORT_GLKTEXTURELOADER */

std::shared_ptr<BGE::Error> BGE::Texture::initialize(TextureHandle handle, std::string name, RawTexture *rawTexture) {
    TextureFormat format = TextureFormat::Undefined;
#ifdef SUPPORT_OPENGL
    GLint glFormat = 0;
#endif /* SUPPORT_OPENGL */
    
    switch (rawTexture->getFormat()) {
        case RawTexture::Format::RGB8_A8:
            format = TextureFormat::RGBA8888;
#ifdef SUPPORT_OPENGL
            glFormat = GL_RGBA;
#endif /* SUPPORT_OPENGL */
            break;
            
        case RawTexture::Format::RGB8:
            format = TextureFormat::RGB888;
#ifdef SUPPORT_OPENGL
            glFormat = GL_RGB;
#endif /* SUPPORT_OPENGL */
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
#ifdef DEBUG_RENDER_COMMAND
    RenderTextureCommandData data(name, getHandle(), getFormat(), buffer, width, height);
#else
    RenderTextureCommandData data(getHandle(), getFormat(), buffer, width, height);
#endif /* DEBUG_RENDER_COMMAND */
    auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
    auto fut = prom->get_future();

#ifdef SUPPORT_OPENGL
    data.glFormat = glFormat;
#endif /* SUPPORT_OPENGL */
    Game::getInstance()->getRenderService()->queueCreateTexture(data, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
        // Do the least amount of work inside
        if (!error) {
            auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
            valid_ = true;
#ifdef SUPPORT_OPENGL
            hwId_ = data->glHwId;
#endif /* SUPPORT_OPENGL */
        } else {
            valid_ = false;
        }
        prom->set_value(error);
    });
    auto error = fut.get();

    if (!error) {
        format_ = format;
#ifdef SUPPORT_OPENGL
        target_ = GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */
        width_ = width;
        height_ = height;

        updateUVs();
        updateXYs();

        memoryUsage_ = computeMemoryUsage(format_, width_, height_);
    }

    return error;
}

void BGE::Texture::destroy() {
    valid_ = false;
    
    if (!isSubTexture_) {
        // Only non-subtextures can be freed, since the atlas is freed separately
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData data(getName(), getHandle());
#else
        RenderTextureCommandData data(getHandle());
#endif /* DEBUG_RENDER_COMMAND */
#ifdef SUPPORT_OPENGL
        GLuint name;

#ifdef SUPPORT_GLKTEXTURELOADER
        if (textureInfo_) {
            name = textureInfo_.name;
        } else {
            name = hwId_;
        }
#else
        name = hwId_;
#endif /* SUPPORT_GLKTEXTURELOADER */
        data.glHwId = name;
#endif /* SUPPORT_OPENGL */

        Game::getInstance()->getRenderService()->queueDestroyTexture(data, [](RenderCommandItem, __attribute__((unused)) std::shared_ptr<Error> error) {
        });
#ifdef SUPPORT_OPENGL
        if (vboId_) {
            RenderVboCommandData vboData(vboId_);
            Game::getInstance()->getRenderService()->queueDestroyVbo(vboData, [](RenderCommandItem, __attribute__((unused)) std::shared_ptr<Error> error) {
            });
        }
        if (iboId_) {
            RenderIboCommandData iboData(iboId_);
            Game::getInstance()->getRenderService()->queueDestroyIbo(iboData, [](RenderCommandItem, __attribute__((unused)) std::shared_ptr<Error> error) {
            });
        }
#endif /* SUPPORT_OPENGL */
    }

#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    vboId_ = 0;
    iboId_ = 0;
    iboOffset_ = 0;
#endif /* SUPPORT_OPENGL */
#ifdef SUPPORT_GLKTEXTURELOADER
    textureInfo_ = nil;
#endif /* SUPPORT_GLKTEXTURELOADER */
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
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        float w_2 = getWidth() / 2.0;
        float h_2 = getHeight() / 2.0;
        xys_[0].x = -w_2;
        xys_[0].y = h_2;
        
        xys_[1].x = w_2;
        xys_[1].y = h_2;
        
        xys_[2].x = w_2;
        xys_[2].y = -h_2;
        
        xys_[3].x = -w_2;
        xys_[3].y = -h_2;
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
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        float w_2 = getWidth() / 2.0;
        float h_2 = getHeight() / 2.0;

        xys_[0].x = -w_2;
        xys_[0].y = -h_2;
        
        xys_[1].x = w_2;
        xys_[1].y = -h_2;
        
        xys_[2].x = w_2;
        xys_[2].y = h_2;
        
        xys_[3].x = -w_2;
        xys_[3].y = h_2;
    }
}

void BGE::Texture::updateXYsForFont() {
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
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        float w = getWidth();
        float h = getHeight();
        xys_[0].x = 0;
        xys_[0].y = h;
        
        xys_[1].x = w;
        xys_[1].y = h;
        
        xys_[2].x = w;
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
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        float w = getWidth();
        float h = getHeight();
        
        xys_[0].x = 0;
        xys_[0].y = 0;
        
        xys_[1].x = w;
        xys_[1].y = 0;
        
        xys_[2].x = w;
        xys_[2].y = h;
        
        xys_[3].x = 0;
        xys_[3].y = h;
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

std::shared_ptr<BGE::Error> BGE::Texture::createSubTexture(TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated, bool font) {
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

#ifdef SUPPORT_OPENGL
            hwId_ = atlas->getHWTextureId();
            target_ = atlas->getTarget();
#endif /* SUPPORT_OPENGL */

            updateUVs(rotated);
            if (font) {
                updateXYsForFont();
            } else {
                updateXYs();
            }
            
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
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData data(getName(), getHandle(), getFormat(), buffer, width, height);
#else
        RenderTextureCommandData data(getHandle(), getFormat(), buffer, width, height);
#endif /* DEBUG_RENDER_COMMAND */
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
#ifdef SUPPORT_OPENGL
        data.glFormat = GL_ALPHA;
#endif /* SUPPORT_OPENGL */
        Game::getInstance()->getRenderService()->queueCreateTexture(data, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
#ifdef SUPPORT_OPENGL
                hwId_ = data->glHwId;
#endif /* SUPPORT_OPENGL */
            }
            prom->set_value(error);
        });
        return fut.get();
    } else {
        return std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGB565Buffer(__attribute__ ((unused)) unsigned char *buffer, __attribute__ ((unused)) uint32_t width, __attribute__ ((unused)) uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    if (buffer) {
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData data(getName(), getHandle(), getFormat(), buffer, width, height);
#else
        RenderTextureCommandData data(getHandle(), getFormat(), buffer, width, height);
#endif /* DEBUG_RENDER_COMMAND */
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
#ifdef SUPPORT_OPENGL
        data.glFormat = GL_RGB;
#endif /* SUPPORT_OPENGL */
        Game::getInstance()->getRenderService()->queueCreateTexture(data, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
#ifdef SUPPORT_OPENGL
                hwId_ = data->glHwId;
#endif /* SUPPORT_OPENGL */
            }
            prom->set_value(error);
        });
        return fut.get();
    } else {
        return std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA5551Buffer(__attribute__ ((unused)) unsigned char *buffer, __attribute__ ((unused)) uint32_t width, __attribute__ ((unused)) uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA4444Buffer(__attribute__ ((unused)) unsigned char *buffer, __attribute__ ((unused)) uint32_t width, __attribute__ ((unused)) uint32_t height) {
    return std::make_shared<Error>(ErrorDomain, TextureErrorUnsupported);
}

std::shared_ptr<BGE::Error> BGE::Texture::createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height) {
    if (buffer) {
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData data(getName(), getHandle(), getFormat(), buffer, width, height);
#else
        RenderTextureCommandData data(getHandle(), getFormat(), buffer, width, height);
#endif /* DEBUG_RENDER_COMMAND */
        auto prom = std::make_shared<std::promise<std::shared_ptr<Error>>>();
        auto fut = prom->get_future();
#ifdef SUPPORT_OPENGL
        data.glFormat = GL_RGBA;
#endif /* SUPPORT_OPENGL */
        Game::getInstance()->getRenderService()->queueCreateTexture(data, [this, prom](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                valid_ = true;
#ifdef SUPPORT_OPENGL
                hwId_ = data->glHwId;
#endif /* SUPPORT_OPENGL */
            }
            prom->set_value(error);
        });
        return fut.get();
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

