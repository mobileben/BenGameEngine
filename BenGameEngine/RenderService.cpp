//
//  RenderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderService.h"

BGE::RenderService::RenderService() : ready_(false)
{
}

void BGE::RenderService::bindRenderWindow(std::shared_ptr<BGERenderContext> context, std::shared_ptr<BGERenderWindow> window)
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

void BGE::RenderService::setCoordinateSystem2D(BGERender2DCoordinateSystem coordSystem2D)
{
    coordSystem2D_ = coordSystem2D;
    
    switch (coordSystem2D) {
        case BGERender2DCoordinateSystem::TraditionalCentered:
        case BGERender2DCoordinateSystem::Traditional:
            invertedYAxis_ = false;
            break;
            
        case BGERender2DCoordinateSystem::OpenGLCentered:
        case BGERender2DCoordinateSystem::OpenGL:
            invertedYAxis_ = true;
            break;
    }
}

