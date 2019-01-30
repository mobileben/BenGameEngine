//
//  RenderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderService.h"
#include "FontService.h"

#if DEBUG
#include <pthread.h>
#endif

const std::string BGE::RenderService::ErrorDomain = "RenderService";

BGE::RenderService::RenderService() : ready_(false), backgroundColor_({{0, 0, 0, 1}}) {
    thread_ = std::thread(&RenderService::threadFunction, this);
}

void BGE::RenderService::bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window)
{
    this->renderContext_ = context;
    this->renderWindow_ = window;
    window->setRenderContext(context);
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
    auto data = std::make_shared<RenderTextureCommandData>(texData.textureHandle, texData.textureFormat, texData.textureBuffer, texData.textureWidth, texData.textureHeight);
#ifdef SUPPORT_OPENGL
    data->glFormat = texData.glFormat;
#endif /* SUPPORT_OPENGL */
    auto command = RenderCommandItem(RenderCommand::TextureCreate, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueDestroyTexture(const RenderTextureCommandData& texData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) {
    auto data = std::make_shared<RenderTextureCommandData>(texData.textureHandle);
#ifdef SUPPORT_OPENGL
    data->glHwId = texData.glHwId;
#endif /* SUPPORT_OPENGL */
    auto command = RenderCommandItem(RenderCommand::TextureDestroy, data, callback);
    renderQueue_.push(command);
}

void BGE::RenderService::queueRender() {
    auto command = RenderCommandItem(RenderCommand::Render);
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

void BGE::RenderService::threadFunction() {
#if DEBUG
    auto native = thread_.native_handle();
    if (native == pthread_self()) {
        pthread_setname_np("render");
    }
#endif
    while(true) {
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

            case RenderCommand::Render:
                render();
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
    numProcessedObjects_ = 0;
    processingTime_ = 0;
}

#endif /* SUPPORT_PROFILING */



