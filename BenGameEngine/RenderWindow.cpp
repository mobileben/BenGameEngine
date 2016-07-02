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

BGE::RenderWindow::~RenderWindow()
{
}

void BGE::RenderWindow::setView(GLKView *view) {
    if (view != this->view_) {
        this->view_ = view;
        
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
    if (this->view_) {
        return this->view_.frame.origin.x;
    } else {
        return 0;
    }
}

float BGE::RenderWindow::getY() const {
    if (this->view_) {
        return this->view_.frame.origin.y;
    } else {
        return 0;
    }
}

float BGE::RenderWindow::getWidth() const {
    if (this->view_) {
        return this->view_.frame.size.width;
    } else {
        return 0;
    }
}

float BGE::RenderWindow::getHeight() const {
    if (this->view_) {
        return this->view_.frame.size.height;
    } else {
        return 0;
    }
}

float BGE::RenderWindow::getContentScaleFactor() const {
    if (this->view_) {
        return this->view_.contentScaleFactor;
    } else {
        return 1;
    }
}
