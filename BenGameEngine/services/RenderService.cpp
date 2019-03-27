//
//  RenderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderService.h"
#include "FontService.h"
#include "Space.h"
#include "TextComponent.h"

#if DEBUG
#include <pthread.h>
#endif

const std::string BGE::RenderService::ErrorDomain = "RenderService";

BGE::RenderService::RenderService() : ready_(false), backgroundColor_({{0, 0, 0, 1}}), threadRunning_(false) {
}

BGE::RenderService::~RenderService() {
    std::lock_guard<std::mutex> lock(threadRunningMutex_);
    threadRunning_ = false;
    renderQueue_.quit();
    try {
        thread_.join();
    } catch(std::exception& e) {
        printf("Exception trying to join thread %s\n", e.what());
    }
}

void BGE::RenderService::initialize() {
    thread_ = std::thread(&RenderService::threadFunction, this);
}

void BGE::RenderService::bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window)
{
    this->renderContext_ = context;
    this->renderWindow_ = window;
    window->setRenderContext(context);
    
    windowMappedDimensionsUpdated(window);
}

void BGE::RenderService::resizeRenderWindow()
{
}

void BGE::RenderService::createShaders()
{
}

void BGE::RenderService::setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D)
{
    coordSystem2D_ = coordSystem2D;
    
    switch (coordSystem2D) {
        case Render2DCoordinateSystem::TraditionalCentered:
        case Render2DCoordinateSystem::Traditional:
            invertedYAxis_ = false;
            break;
            
        case Render2DCoordinateSystem::OpenGLCentered:
        case Render2DCoordinateSystem::OpenGL:
            invertedYAxis_ = true;
            break;
    }
}

BGE::Vector2 BGE::RenderService::getWindowWidthHeight() const {
    Vector2 wh;
    auto window = getRenderWindow();
    
    wh.w = window->getWidth() * window->getContentScaleFactor();
    wh.h = window->getHeight() * window->getContentScaleFactor();
    
    return wh;
}

BGE::Vector2 BGE::RenderService::getWindowMappedWidthHeight() const {
    Vector2 wh;
    auto window = getRenderWindow();
    
    wh.w = window->getMappedWidth();
    wh.h = window->getMappedHeight();
    
    return wh;
}

void BGE::RenderService::windowMappedDimensionsUpdated(__attribute__((unused)) std::shared_ptr<RenderWindow> window) {
    setCoordinateSystem2D(getCoordinateSystem2D());
    
    // Now notify shaders through Shader Service
    getShaderService()->windowMappedDimensionsUpdated();
}

BGE::Vector2 BGE::RenderService::deviceCoordinatesFromRenderCoordinates(Vector2 pos) {
    // This is the MacOS/iOS implementation
    Vector2 convertedPos;
    auto window = this->getRenderWindow();
    auto scale = window->getContentScaleFactor();
    
    switch (coordSystem2D_) {
        case Render2DCoordinateSystem::Traditional:
            convertedPos.x =  pos.x / scale;
            convertedPos.y =  pos.y / scale;
            break;
        case Render2DCoordinateSystem::TraditionalCentered:
            convertedPos.x =  pos.x / scale + window->getWidth()/2;
            convertedPos.y =  pos.y / scale + window->getHeight()/2;
            break;
        case Render2DCoordinateSystem::OpenGL:
            convertedPos.x =  pos.x / scale;
            convertedPos.y =  window->getHeight() - pos.y / scale;
            break;
        case Render2DCoordinateSystem::OpenGLCentered:
            convertedPos.x = pos.x / scale + window->getWidth() / 2;
            convertedPos.y = window->getHeight() / 2 - pos.y / scale;
            break;
    }
    
    return convertedPos;
}

void BGE::RenderService::setComponentService(std::shared_ptr<ComponentService> componentService) {
    componentService_ = componentService;
}

void BGE::RenderService::queueBindRenderWindow(const std::shared_ptr<RenderContext>& context, const std::shared_ptr<RenderWindow>& window) {
    auto data = std::make_shared<RenderCommandBindWindowData>(context, window);
    auto command = RenderCommandItem(RenderCommand::BindWindow, data);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreateBuiltinShaders() {
    auto command = RenderCommandItem(RenderCommand::CreateBuiltinShaders);
    renderQueue_.push(command);
}

void BGE::RenderService::queueSetIsReady() {
    auto command = RenderCommandItem(RenderCommand::SetIsReady);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreateTexture(const RenderTextureCommandData& texData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderTextureCommandData>(texData);
#ifdef SUPPORT_OPENGL
    data->glFormat = texData.glFormat;
#endif /* SUPPORT_OPENGL */
    auto command = RenderCommandItem(RenderCommand::TextureCreate, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyTexture(const RenderTextureCommandData& texData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderTextureCommandData>(texData);
#ifdef SUPPORT_OPENGL
    data->glHwId = texData.glHwId;
#endif /* SUPPORT_OPENGL */
    auto command = RenderCommandItem(RenderCommand::TextureDestroy, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreateVbo(const RenderVboCommandData& vboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderVboCommandData>(vboData);
    auto command = RenderCommandItem(RenderCommand::VboCreate, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyVbo(const RenderVboCommandData& vboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderVboCommandData>(vboData);
    auto command = RenderCommandItem(RenderCommand::VboDestroy, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreateIbo(const RenderIboCommandData& iboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderIboCommandData>(iboData);
    auto command = RenderCommandItem(RenderCommand::IboCreate, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyIbo(const RenderIboCommandData& iboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderIboCommandData>(iboData);
    auto command = RenderCommandItem(RenderCommand::IboDestroy, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreateStringCacheEntry(const RenderStringCacheCommandData& cacheData) {
    auto data = std::make_shared<RenderStringCacheCommandData>(cacheData);
    auto command = RenderCommandItem(RenderCommand::CreateStringCacheEntry, data);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyStringCacheEntry(const RenderStringCacheCommandData& cacheData) {
    auto data = std::make_shared<RenderStringCacheCommandData>(cacheData);
    auto command = RenderCommandItem(RenderCommand::DestroyStringCacheEntry, data);
    renderQueue_.push(command);
}

void BGE::RenderService::queueCreatePolyLineCacheEntry(const RenderPolyLineCacheCommandData& cacheData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderPolyLineCacheCommandData>(cacheData);
    auto command = RenderCommandItem(RenderCommand::CreatePolyLineCacheEntry, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyPolyLineCacheEntry(const RenderPolyLineCacheCommandData& cacheData) {
    auto data = std::make_shared<RenderPolyLineCacheCommandData>(cacheData);
    auto command = RenderCommandItem(RenderCommand::DestroyPolyLineCacheEntry, data);
    renderQueue_.push(command);
}

void BGE::RenderService::queueRender(std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto command = RenderCommandItem(RenderCommand::Render, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::createTexture(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::destroyTexture(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::createVbo(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::destroyVbo(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::createIbo(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::destroyIbo(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::createStringCacheEntry(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::destroyStringCacheEntry(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::createPolyLineCacheEntry(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

void BGE::RenderService::destroyPolyLineCacheEntry(const RenderCommandItem& item) {
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

BGE::SpaceHandle BGE::RenderService::getSpaceHandleFromCachedComponentRenderDataKey(CachedComponentRenderDataKey key) {
    return SpaceHandle(static_cast<HandleBackingType>((key >> 32)&0xFFFFFFFF));
}

void BGE::RenderService::threadFunction() {
    pthread_setname_np("render");
    threadRunning_ = true;

    // TODO: Make threadRunning_ protected by a mutex
    while(true) {
        std::unique_lock<std::mutex> lock(threadRunningMutex_);
        if (!threadRunning_) {
            lock.unlock();
            threadCleanup();
            return;
        }
        lock.unlock();
        
        auto command = renderQueue_.pop();
        switch (command.command) {
            case RenderCommand::BindWindow: {
                std::shared_ptr<RenderCommandBindWindowData> data = std::dynamic_pointer_cast<RenderCommandBindWindowData>(command.data);
                if (data) {
                    bindRenderWindow(data->context, data->window);
                    if (command.callback) {
                        command.callback(command, nullptr);
                    }
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
            case RenderCommand::CreateBuiltinShaders:
                createShaders();
                if (command.callback) {
                    command.callback(command, nullptr);
                }
                break;

            case RenderCommand::SetIsReady:
                setIsReady();
                if (command.callback) {
                    command.callback(command, nullptr);
                }
                break;

            case RenderCommand::TextureCreate: {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                if (data) {
                    createTexture(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;

            case RenderCommand::TextureDestroy: {
                auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(command.data);
                if (data) {
                    destroyTexture(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;

            case RenderCommand::VboCreate: {
                auto data = std::dynamic_pointer_cast<RenderVboCommandData>(command.data);
                if (data) {
                    createVbo(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::VboDestroy: {
                auto data = std::dynamic_pointer_cast<RenderVboCommandData>(command.data);
                if (data) {
                    destroyVbo(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::IboCreate: {
                auto data = std::dynamic_pointer_cast<RenderIboCommandData>(command.data);
                if (data) {
                    createIbo(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::IboDestroy: {
                auto data = std::dynamic_pointer_cast<RenderIboCommandData>(command.data);
                if (data) {
                    destroyIbo(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::CreateStringCacheEntry: {
                auto data = std::dynamic_pointer_cast<RenderStringCacheCommandData>(command.data);
                if (data) {
                    createStringCacheEntry(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::DestroyStringCacheEntry: {
                auto data = std::dynamic_pointer_cast<RenderStringCacheCommandData>(command.data);
                if (data) {
                    destroyStringCacheEntry(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::CreatePolyLineCacheEntry: {
                auto data = std::dynamic_pointer_cast<RenderPolyLineCacheCommandData>(command.data);
                if (data) {
                    createPolyLineCacheEntry(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::DestroyPolyLineCacheEntry: {
                auto data = std::dynamic_pointer_cast<RenderPolyLineCacheCommandData>(command.data);
                if (data) {
                    destroyPolyLineCacheEntry(command);
                } else if (command.callback) {
                    // TODO: We failed :(
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::RenderCommandMissingData)));
                }
            }
                break;
                
            case RenderCommand::Render:
                render();
                if (command.callback) {
                    command.callback(command, nullptr);
                }
                break;
                
            default:
                if (command.callback) {
                    command.callback(command, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unsupported)));
                }
                break;

        }
    }
}

#ifdef SUPPORT_PROFILING

void BGE::RenderService::resetProfilingStats() {
    numGameObjectsDrawn_ = 0;
    numGameObjectsIgnored_ = 0;
    numDrawCalls_ = 0;
    numSpritesDrawn_ = 0;
    numFontCharactersDrawn_ = 0;
    numPolylinesDrawn_ = 0;
    numLinesDrawn_= 0;
    numRectsDrawn_ = 0;
    numMasksDrawn_ = 0;
    numShadersChanged_ = 0;
    numTexturesChanged_ = 0;
    numBlendChanged_ = 0;
    numBlendFuncChanged_ = 0;
    numProcessedObjects_ = 0;
    processingTime_ = 0;
}

#endif /* SUPPORT_PROFILING */



