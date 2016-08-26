//
//  RenderView.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderView_h
#define BGERenderView_h

#include <stdio.h>
#include <memory>
#include "RenderWindow.h"

namespace BGE {
    class RenderView
    {
    public:
        RenderView(std::shared_ptr<RenderWindow> window, float x, float y, float width, float height);
        ~RenderView() {}
        
        float getX();
        float getY();
        float getWidth();
        float getHeight();
        
        std::weak_ptr<RenderWindow> getWindow();
        
    private:
        float x_;
        float y_;
        float width_;
        float height_;
        std::weak_ptr<RenderWindow> window_;
        
    };
}

#endif /* BGERenderView_h */
