//
//  RenderWindow.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderWindow_h
#define BGERenderWindow_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include <memory>
#include <map>
#include <string>
#include "MathTypes.h"
#include "RenderContext.h"

#if TARGET_OS_IPHONE
#include "BGEView.h"
#endif /* TARGET_OS_IPHONE */

namespace BGE {
    class RenderView;
    
    class RenderWindow : public std::enable_shared_from_this<RenderWindow>
    {
    public:
        static std::string DefaultRenderViewName;
        
        RenderWindow() : x_(0), y_(0), width_(0), height_(0), contentScaleFactor_(1), mappedWidth_(0), mappedHeight_(0), toMappedXScale_(1), toMappedYScale_(1), fromMappedXScale_(1), fromMappedYScale_(1) {
            Matrix4MakeScale(toMappedMatrix_, toMappedXScale_, toMappedYScale_, 1.0);
            Matrix4MakeScale(fromMappedMatrix_, fromMappedXScale_, fromMappedYScale_, 1.0);
        }
        ~RenderWindow() {}

#if TARGET_OS_IPHONE
        BGEView *getView() { return view_; }
#endif /* TARGET_OS_IPHONE */
        
        std::shared_ptr<RenderContext> getRenderContext() { return renderContext_; }
        void setRenderContext(std::shared_ptr<RenderContext> renderContext);
        
        inline float getX() const { return x_; }
        inline float getY() const { return y_; }
        inline float getWidth() const { return width_; }
        inline float getHeight() const { return height_; }
        
        inline float getContentScaleFactor() const { return contentScaleFactor_; }
        
        inline float getNativeWidth() const { return width_ * contentScaleFactor_; }
        inline float getNativeHeight() const { return height_ * contentScaleFactor_; }
        void setMappedWidth(float width);
        void setMappedHeight(float height);
        float getMappedWidth() const { return mappedWidth_; }
        float getMappedHeight() const { return mappedHeight_; }
        
        float getToMappedXScale() const { return toMappedXScale_; }
        float getToMappedYScale() const { return toMappedYScale_; }
        float getFromMappedXScale() const { return fromMappedXScale_; }
        float getFromMappedYScale() const { return fromMappedYScale_; }
        const Matrix4& getToMappedMatrix() const { return toMappedMatrix_; }
        const Matrix4& getFromMappedMatrix() const { return fromMappedMatrix_; }

        void addRenderView(std::string name, std::shared_ptr<RenderView> renderView);
        void removeRenderView(std::string name);
        
        std::shared_ptr<RenderView> getRenderView(std::string name) { return renderViews_[name]; }
        
    protected:
        float x_;
        float y_;
        float width_;
        float height_;
        float contentScaleFactor_;
        
        // Window also stores information related to mapped views when we are dealing
        // with rendering based on a different screen size
        float mappedWidth_;
        float mappedHeight_;
        
        float toMappedXScale_;
        float toMappedYScale_;
        float fromMappedXScale_;
        float fromMappedYScale_;
        
        Matrix4 toMappedMatrix_;
        Matrix4 fromMappedMatrix_;

    private:
#if TARGET_OS_IPHONE
        BGEView *view_;
#endif /* TARGET_OS_IPHONE */
        std::shared_ptr<RenderContext> renderContext_;
        std::map<std::string, std::shared_ptr<RenderView>> renderViews_;
    };
}

#endif /* BGERenderWindow_h */
