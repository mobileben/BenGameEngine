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
        
        RenderWindow() : x_(0), y_(0), width_(0), height_(0), contentScaleFactor_(1) {}
        ~RenderWindow() {}

#if TARGET_OS_IPHONE
        BGEView *getView() { return view_; }
#endif /* TARGET_OS_IPHONE */
        
        std::shared_ptr<RenderContext> getRenderContext() { return renderContext_; }
        void setRenderContext(std::shared_ptr<RenderContext> renderContext);
        
        float getX() const ;
        float getY() const ;
        float getWidth() const ;
        float getHeight() const ;
        
        float getContentScaleFactor() const;
        
        void addRenderView(std::string name, std::shared_ptr<RenderView> renderView);
        void removeRenderView(std::string name);
        
        std::shared_ptr<RenderView> getRenderView(std::string name) { return renderViews_[name]; }
        
    protected:
        float x_;
        float y_;
        float width_;
        float height_;
        float contentScaleFactor_;
        
    private:
#if TARGET_OS_IPHONE
        BGEView *view_;
#endif /* TARGET_OS_IPHONE */
        std::shared_ptr<RenderContext> renderContext_;
        std::map<std::string, std::shared_ptr<RenderView>> renderViews_;
    };
}

#endif /* BGERenderWindow_h */
