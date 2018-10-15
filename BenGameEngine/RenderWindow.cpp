//
//  RenderWindow.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#include "BGERenderViewIOS.h"
#endif /* TARGET_OS_IPHONE */

#include "RenderWindow.h"
#include "RenderView.h"

std::string BGE::RenderWindow::DefaultRenderViewName = "DefaultRenderView";

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

void BGE::RenderWindow::setMappedWidth(float width) {
    auto nativeWidth = width_ * contentScaleFactor_;
    mappedWidth_ = width;
    toMappedXScale_ = nativeWidth / mappedWidth_;
    fromMappedXScale_ = mappedWidth_ / nativeWidth;
    Matrix4MakeScale(toMappedMatrix_, toMappedXScale_, toMappedYScale_, 1.0);
    Matrix4MakeScale(fromMappedMatrix_, fromMappedXScale_, fromMappedYScale_, 1.0);
}

void BGE::RenderWindow::setMappedHeight(float height) {
    auto nativeHeight = height_ * contentScaleFactor_;
    mappedHeight_ = height;
    toMappedYScale_ = nativeHeight / mappedHeight_;
    fromMappedYScale_ = mappedHeight_ / nativeHeight;
    Matrix4MakeScale(toMappedMatrix_, toMappedXScale_, toMappedYScale_, 1.0);
    Matrix4MakeScale(fromMappedMatrix_, fromMappedXScale_, fromMappedYScale_, 1.0);
}

void BGE::RenderWindow::addRenderView(std::string name, std::shared_ptr<RenderView> renderView) {
    if (renderViews_.empty()) {
#if TARGET_OS_IPHONE
        std::shared_ptr<BGERenderViewIOS> iosView = std::dynamic_pointer_cast<BGERenderViewIOS>(renderView);
        if (iosView) {
            BGEView *view = iosView->getView();
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
            setMappedWidth(this->width_ * this->contentScaleFactor_);
            setMappedHeight(this->height_ * this->contentScaleFactor_);
            view_ = view;
        }
#endif /* TARGET_OS_IPHONE */
    }
    renderView->window_ = std::weak_ptr<RenderWindow>(shared_from_this());
    renderViews_[name] = renderView;
}

void BGE::RenderWindow::removeRenderView(std::string name) {
    auto it = renderViews_.find(name);
    if (it != renderViews_.end()) {
        auto rView = it->second;
        if (rView) {
            rView->window_.reset();
        }
        renderViews_.erase(it);
    }
}
