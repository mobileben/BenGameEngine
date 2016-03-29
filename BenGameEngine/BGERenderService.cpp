//
//  BGERenderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGERenderService.h"

BGERenderService::BGERenderService() : ready_(false)
{
}

void BGERenderService::bindRenderWindow(std::shared_ptr<BGERenderContext> context, std::shared_ptr<BGERenderWindow> window)
{
    this->renderContext_ = context;
    this->renderWindow_ = window;
    window->setRenderContext(context);
}

void BGERenderService::resizeRenderWindow()
{
}

void BGERenderService::createShaders()
{
}

void BGERenderService::setCoordinateSystem2D(BGERender2DCoordinateSystem coordSystem2D)
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

