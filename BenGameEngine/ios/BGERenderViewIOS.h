//
//  BGERenderViewIOS.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/7/18.
//  Copyright © 2018 2n Productions. All rights reserved.
//

#ifndef BGERenderViewIOS_h
#define BGERenderViewIOS_h

#include "RenderView.h"
#import "BGEView.h"

#include <stdio.h>

#include <memory>

namespace BGE {
    class BGERenderViewIOS : public RenderView {
    public:
        BGERenderViewIOS(BGEView *view);
        ~BGERenderViewIOS() {}
        
        BGEView *getView() const { return view; }
        
    private:
        BGEView *view;
    };
}

#endif /* BGERenderViewIOS_h */
