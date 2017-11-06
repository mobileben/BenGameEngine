//
//  RenderWindow.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include "RenderWindow.h"
#include "RenderView.h"

std::string BGE::RenderWindow::DefaultRenderViewName = "DefaultRenderView";

void BGE::RenderWindow::setView(BGEView *view) {
    if (view != this->view_) {
        this->view_ = view;
        if (view) {
            this->x_ = view.frame.origin.x;
            this->y_ = view.frame.origin.y;
            this->width_ = view.frame.size.width;
            this->height_ = view.frame.size.height;
            this->contentScaleFactor_ = view.contentScaleFactor;
        } else {
            this->x_ = 0;
            this->y_ = 0;
            this->width_ = 0;
            this->height_ = 0;
            this->contentScaleFactor_ = 1;
        }

        this->renderViews_.clear();

        if (this->view_) {
            // Create the default RenderView
            this->renderViews_[DefaultRenderViewName] = std::make_shared<RenderView>(shared_from_this(), getX(), getY(), getWidth(), getHeight());
            
        }
    }
}

void BGE::RenderWindow::setRenderContext(std::shared_ptr<RenderContext> renderContext)
{
    renderContext_ = renderContext;
}

float BGE::RenderWindow::getX() const {
    return x_;
}

float BGE::RenderWindow::getY() const {
    return y_;
}

float BGE::RenderWindow::getWidth() const {
    return width_;
}

float BGE::RenderWindow::getHeight() const {
    return height_;
}

float BGE::RenderWindow::getContentScaleFactor() const {
    return contentScaleFactor_;
}
