//
//  BGERenderWindow.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include "BGERenderWindow.h"
#include "BGERenderView.h"

std::string BGERenderWindow::DefaultRenderViewName = "DefaultRenderView";

BGERenderWindow::~BGERenderWindow()
{
}

void BGERenderWindow::setView(GLKView *view) {
    if (view != this->view_) {
        this->view_ = view;
        
        this->renderViews_.clear();

        if (this->view_) {
            // Create the default BGERenderView
            this->renderViews_[DefaultRenderViewName] = std::make_shared<BGERenderView>(shared_from_this(), getX(), getY(), getWidth(), getHeight());
            
        }
    }
}

void BGERenderWindow::setRenderContext(std::shared_ptr<BGERenderContext> renderContext)
{
    renderContext_ = renderContext;
}

float BGERenderWindow::getX() const {
    if (this->view_) {
        return this->view_.frame.origin.x;
    } else {
        return 0;
    }
}

float BGERenderWindow::getY() const {
    if (this->view_) {
        return this->view_.frame.origin.y;
    } else {
        return 0;
    }
}

float BGERenderWindow::getWidth() const {
    if (this->view_) {
        return this->view_.frame.size.width;
    } else {
        return 0;
    }
}

float BGERenderWindow::getHeight() const {
    if (this->view_) {
        return this->view_.frame.size.height;
    } else {
        return 0;
    }
}

float BGERenderWindow::getContentScaleFactor() const {
    if (this->view_) {
        return this->view_.contentScaleFactor;
    } else {
        return 1;
    }
}
