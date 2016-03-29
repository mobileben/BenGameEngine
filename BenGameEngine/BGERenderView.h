//
//  BGERenderView.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGERenderView_h
#define BGERenderView_h

#include <stdio.h>
#include <memory>
#include "BGERenderWindow.h"

class BGERenderView
{
public:
    BGERenderView(std::shared_ptr<BGERenderWindow> window, float x, float y, float width, float height);
    ~BGERenderView();
    
    float getX();
    float getY();
    float getWidth();
    float getHeight();

    std::weak_ptr<BGERenderWindow> getWindow();
    
private:
    float x_;
    float y_;
    float width_;
    float height_;
    std::weak_ptr<BGERenderWindow> window_;
    
};
#endif /* BGERenderView_h */
