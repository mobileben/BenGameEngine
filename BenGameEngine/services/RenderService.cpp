//
//  RenderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderService.h"
#include "FontService.h"

BGE::RenderService::RenderService() : ready_(false), backgroundColor_({0, 0, 0, 1}) {
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


