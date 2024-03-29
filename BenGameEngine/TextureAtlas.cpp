//
//  TextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureAtlas.h"
#include "Game.h"

#include <future>

const std::string BGE::TextureAtlas::ErrorDomain = "TextureAtlas";

BGE::TextureAtlas::TextureAtlas() : NamedObject(), valid_(false), width_(0), height_(0), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None) {
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_= GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */
}

BGE::TextureAtlas::TextureAtlas(ObjectId texId) : NamedObject(texId), valid_(false), width_(0), height_(0), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None) {
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_= GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */
}

BGE::TextureAtlas::TextureAtlas(ObjectId texId, std::string name) : NamedObject(texId, name), valid_(false), width_(0), height_(0), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None) {
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_= GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */
}

void BGE::TextureAtlas::reset() {
    valid_ = false;
    handle_ = TextureAtlasHandle();
    textureHandle_ = TextureHandle();
    
    format_ = TextureFormat::Undefined;
    alphaState_ = TextureAlphaState::None;
    width_ = 0;
    height_ = 0;
#ifdef SUPPORT_OPENGL
    hwId_ = 0;
    target_ = GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */
    
    subTextures_.clear();
}

void BGE::TextureAtlas::initialize(TextureAtlasHandle handle, std::string name) {
    reset();
    
    handle_ = handle;
    setName(name);
}

void BGE::TextureAtlas::destroy() {
    auto textureService = Game::getInstance()->getTextureService();
    
    valid_ = false;
    
    for (auto const &it : subTextures_) {
        textureService->removeTexture(handle_, it.second);
    }
    
    subTextures_.clear();
    
    // Now remove underlying texture
    textureService->removeTexture(handle_, textureHandle_);
    
    handle_ = TextureAtlasHandle();
    textureHandle_ = TextureHandle();
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

BGE::Texture *BGE::TextureAtlas::getSubTexture(std::string name) {
    auto handle = getSubTextureHandle(name);
    
    return BGE::Game::getInstance()->getTextureService()->getTexture(handle);
}

size_t BGE::TextureAtlas::getMemoryUsage() const {
    auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle_);
    
    if (texture) {
        return texture->getMemoryUsage();
    }
    
    return 0;
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureAtlas::createFromFile(std::string filename, std::vector<SubTextureDef> &subTextures, TextureFormat format, bool createVbo) {
    auto textureService = Game::getInstance()->getTextureService();
    Texture *texture;
    std::shared_ptr<Error> error;
    std::tie(texture, error) = textureService->createTextureFromFile(getHandle(), atlasTextureKey(), filename, format);
    if (!error) {
        TextureAtlas *atlas = this;

        if (texture) {
            
            // Texture needs to be set before processing subTex
            this->textureHandle_ = texture->getHandle();

            this->valid_ = true;
            this->format_ = texture->getFormat();
            this->alphaState_ = texture->getAlphaState();
            this->width_ = texture->getWidth();
            this->height_ = texture->getHeight();
#ifdef SUPPORT_OPENGL
            this->hwId_ = texture->getHWTextureId();
            this->target_ = texture->getTarget();
#endif /* SUPPORT_OPENGL */

            if (subTextures.size() > 0) {
                for (auto const &st : subTextures) {
                    std::string key = st.name;
                    auto subTex = textureService->createSubTexture(getHandle(), key, this, st.x, st.y, st.width, st.height, st.rotated, st.font);

                    if (subTex) {
                        this->subTextures_[key] = subTex->getHandle();
                    }
                }
                
                if (createVbo) {
                    // Refresh texture just in case, since there have been times it has gone stale
                    texture = textureService->getTexture(getTextureHandle());
                    buildVertexTexData(texture);
                }
            } else {
                valid_ = false;
                format_ = TextureFormat::Undefined;
                alphaState_ = TextureAlphaState::None;
                width_ = 0;
                height_ = 0;
#ifdef SUPPORT_OPENGL
                hwId_ = 0;
                target_ = GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */

                atlas = nullptr;
                error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorInvalidSubTexture);
            }
        } else {
            atlas = nullptr;
            error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorExistingTextureWrongType);
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(nullptr, error);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureAtlas::createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextures, bool createVbo) {
    Texture *texture;
    std::shared_ptr<Error> error;
    auto textureService = Game::getInstance()->getTextureService();
    std::tie(texture, error) = textureService->createTextureFromBuffer(getHandle(), atlasTextureKey(), buffer, format, width, height);
    if (!error) {
        TextureAtlas *atlas = this;
        if (texture) {
            // Texture needs to be set before processing subTex
            this->textureHandle_ = texture->getHandle();

            this->valid_ = true;
            this->format_ = texture->getFormat();
            this->alphaState_ = texture->getAlphaState();
            this->width_ = texture->getWidth();
            this->height_ = texture->getHeight();
#ifdef SUPPORT_OPENGL
            this->hwId_ = texture->getHWTextureId();
            this->target_ = texture->getTarget();
#endif /* SUPPORT_OPENGL */

            if (subTextures.size() > 0) {
                for (auto const &st : subTextures) {
                    std::string key = st.name;
                    auto subTex = textureService->createSubTexture(getHandle(), key, this, st.x, st.y, st.width, st.height, st.rotated, st.font);

                    if (subTex) {
                        subTextures_[key] = subTex->getHandle();
                    }
                }
                if (createVbo) {
                    // Refresh texture just in case, since there have been times it has gone stale
                    texture = textureService->getTexture(getTextureHandle());
                    buildVertexTexData(texture);
                }
            } else {
                valid_ = false;
                format_ = TextureFormat::Undefined;
                alphaState_ = TextureAlphaState::None;
                width_ = 0;
                height_ = 0;
#ifdef SUPPORT_OPENGL
                hwId_ = 0;
                target_ = GL_TEXTURE_2D;
#endif /* SUPPORT_OPENGL */

                atlas = nullptr;
                error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorInvalidSubTexture);
            }
        } else {
            atlas = nullptr;
            error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorExistingTextureWrongType);
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(nullptr, error);
    }
}

const std::map<std::string, BGE::TextureHandle>& BGE::TextureAtlas::getSubTextures() const {
    return subTextures_;
}

static uint32_t indexOrdering[] = { 0, 3, 2, 2, 1, 0 };
static uint32_t kNumVertices = 4;

void BGE::TextureAtlas::buildVertexTexData(Texture *texture) {
    if (texture) {
        auto handle = texture->getHandle();
        auto renderService = Game::getInstance()->getRenderService();
        auto textureService = Game::getInstance()->getTextureService();
        uint32_t dataIndex = 0;
#ifdef SUPPORT_OPENGL
        // We need to protect the lifetime of the vectors, so create shared_ptr versions
        std::shared_ptr<std::vector<VertexTex>> vertexTexDataShared = std::make_shared<std::vector<VertexTex>>();
        auto& vertexTexData = *vertexTexDataShared;
        
        // We need to protect the lifetime of the vectors, so create shared_ptr versions
        std::shared_ptr<std::vector<GLushort>>     vertexIndicesShared = std::make_shared<std::vector<GLushort>>();
        auto& vertexIndices = *vertexIndicesShared;
        std::map<std::string, GLsizei> subTexVertexIndicesOffset;
        
        static_assert(sizeof(GLushort) == sizeof(texture->vboIndices_[0]), "Mismatch in index sizes");
        vertexIndices.reserve(subTextures_.size() * sizeof(indexOrdering)/sizeof(uint32_t));

        VertexTex vt;
        vt.position.z = 0.0;
        
        for (auto it : subTextures_) {
            auto handle = it.second;
            auto subTex = textureService->getTexture(handle);
            if (subTex) {
                for (size_t i=0;i<kNumVertices;++i) {
                    vt.position.x = subTex->xys_[i].x;
                    vt.position.y = subTex->xys_[i].y;
                    vt.tex.x = subTex->uvs_[i].x;
                    vt.tex.y = subTex->uvs_[i].y;
                    vertexTexData.push_back(vt);
                }
                subTexVertexIndicesOffset[subTex->getName()] = static_cast<GLsizei>(vertexIndices.size() * subTex->getVboIndexSize());
                for (size_t i=0;i<sizeof(indexOrdering)/sizeof(uint32_t);++i) {
                    auto index = static_cast<GLushort>(dataIndex + indexOrdering[i]);
                    subTex->vboIndices_[i] = index;
                    vertexIndices.push_back(index);
                }
                dataIndex += kNumVertices;
            }
        }
#endif /* SUPPORT_OPENGL */

        RenderVboCommandData data(&vertexTexData[0], static_cast<uint32_t>(vertexTexData.size()));
        // We use lambda capture (which does not optimize out the variable) to maintain the lifetime of the vector
        renderService->queueCreateVbo(data, [this, vertexTexDataShared, handle](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
#ifdef SUPPORT_OPENGL
                std::async(std::launch::async, [this, handle, command]() {
                    auto data = std::dynamic_pointer_cast<RenderVboCommandData>(command.data);
                    auto textureService = Game::getInstance()->getTextureService();
                    auto texture = textureService->getTexture(handle);
                    if (data->glVboId) {
                        for (auto it : subTextures_) {
                            auto subTex = textureService->getTexture(it.second);
                            if (subTex) {
                                subTex->vboId_ = data->glVboId;
                            }
                        }
                        texture->vboId_ = data->glVboId;
                    }
                });
#endif /* SUPPORT_OPENGL */
            }
        });
        
#ifdef SUPPORT_OPENGL
        auto texture = textureService->getTexture(handle);
        RenderIboCommandData iboData(&vertexIndices[0], static_cast<uint32_t>(vertexIndices.size()), static_cast<uint32_t>(texture->getVboIndexSize()));
        // We use lambda capture (which does not optimize out the variable) to maintain the lifetime of the vector
        renderService->queueCreateIbo(iboData, [this, vertexIndicesShared, handle, subTexVertexIndicesOffset](RenderCommandItem command, std::shared_ptr<Error> error) {
            if (!error) {
#ifdef SUPPORT_OPENGL
                std::async(std::launch::async, [this, subTexVertexIndicesOffset, handle, command]() {
                    auto data = std::dynamic_pointer_cast<RenderIboCommandData>(command.data);
                    auto textureService = Game::getInstance()->getTextureService();
                    if (data->glIboId) {
                        for (auto it : subTextures_) {
                            auto subTex = textureService->getTexture(it.second);
                            if (subTex) {
                                auto sit = subTexVertexIndicesOffset.find(subTex->getName());
                                if (sit != subTexVertexIndicesOffset.end()) {
                                    subTex->iboId_ = data->glIboId;
                                    subTex->iboOffset_ = sit->second;
                                }
                            }
                        }
                        auto texture = textureService->getTexture(handle);
                        texture->iboId_ = data->glIboId;
                    }
                });
#endif /* SUPPORT_OPENGL */
            }
        });
#endif /* SUPPORT_OPENGL */
    }
}
