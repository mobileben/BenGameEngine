//
//  BGERenderView.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGERenderView.h"

BGERenderView::BGERenderView(std::shared_ptr<BGERenderWindow> window, float x, float y, float width, float height) : window_(window), x_(x), y_(y), width_(width), height_(height)
{
    
}

BGERenderView::~BGERenderView()
{    
}

float BGERenderView::getX()
{
    return x_;
}

float BGERenderView::getY()
{
    return y_;
}

float BGERenderView::getWidth()
{
    return width_;
}

float BGERenderView::getHeight()
{
    return height_;
}

std::weak_ptr<BGERenderWindow> BGERenderView::getWindow()
{
    return window_;
}


