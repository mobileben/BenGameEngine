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
        RenderView(float x, float y, float width, float height);
        virtual ~RenderView() {}
        
        float getX();
        float getY();
        float getWidth();
        float getHeight();
        
        void setMappedWidth(float width);
        void setMappedHeight(float height);
        float getMappedWidth() const { return mappedWidth_; }
        float getMappedHeight() const { return mappedHeight_; }
                       
        std::weak_ptr<RenderWindow> getWindow();
        
    private:
        friend class RenderWindow;
        
        float x_;
        float y_;
        float width_;
        float height_;
        float mappedWidth_;
        float mappedHeight_;

        std::weak_ptr<RenderWindow> window_;
    };
}

#endif /* BGERenderView_h */
