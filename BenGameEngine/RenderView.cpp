//
//  RenderView.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderView.h"

BGE::RenderView::RenderView(float x, float y, float width, float height) : x_(x), y_(y), width_(width), height_(height), mappedWidth_(width), mappedHeight_(height)
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

void BGE::RenderView::setMappedWidth(float width) {
    mappedWidth_ = width;
    if (auto win = window_.lock()) {
        win->setMappedWidth(width);
    }
}

void BGE::RenderView::setMappedHeight(float height) {
    mappedHeight_ = height;
    if (auto win = window_.lock()) {
        win->setMappedHeight(height);
    }
}

std::weak_ptr<BGE::RenderWindow> BGE::RenderView::getWindow()
{
    return window_;
}


