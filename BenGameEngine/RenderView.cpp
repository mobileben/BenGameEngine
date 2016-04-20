//
//  RenderView.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderView.h"

BGE::RenderView::RenderView(std::shared_ptr<RenderWindow> window, float x, float y, float width, float height) : window_(window), x_(x), y_(y), width_(width), height_(height)
{
    
}

BGE::RenderView::~RenderView()
{    
}

float BGE::RenderView::getX()
{
    return x_;
}

float BGE::RenderView::getY()
{
    return y_;
}

float BGE::RenderView::getWidth()
{
    return width_;
}

float BGE::RenderView::getHeight()
{
    return height_;
}

std::weak_ptr<BGE::RenderWindow> BGE::RenderView::getWindow()
{
    return window_;
}


