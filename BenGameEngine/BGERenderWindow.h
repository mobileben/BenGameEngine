//
//  BGERenderWindow.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGERenderWindow_h
#define BGERenderWindow_h

#include <stdio.h>
#include <memory>
#include <map>
#include <string>
#include "BGERenderContext.h"
#include <GLKit/GLKit.h>

class BGERenderView;

class BGERenderWindow : public std::enable_shared_from_this<BGERenderWindow>
{
public:
    static std::string DefaultRenderViewName;
    
    ~BGERenderWindow();
    
    GLKView *getView() { return view_; }
    void setView(GLKView *view);
    
    std::shared_ptr<BGERenderContext> getRenderContext() { return renderContext_; }
    void setRenderContext(std::shared_ptr<BGERenderContext> renderContext);
    
    float getX() const ;
    float getY() const ;
    float getWidth() const ;
    float getHeight() const ;
    
    float getContentScaleFactor() const;
    
    void addRenderView(std::string name, std::shared_ptr<BGERenderView> renderView);
    void removeRenderView(std::string name);
    
    std::shared_ptr<BGERenderView> getRenderView(std::string name) { return renderViews_[name]; }
    
protected:
    float x_;
    float y_;
    float width_;
    float height_;
    
private:
    GLKView *view_;
    std::shared_ptr<BGERenderContext> renderContext_;
    std::map<std::string, std::shared_ptr<BGERenderView>> renderViews_;
};

#endif /* BGERenderWindow_h */
