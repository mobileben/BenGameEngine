//
//  Input.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Input.h"

BGE::Input::Input() : Object()  {
}

void BGE::Input::initialize(InputHandle handle) {
    handle_ = handle;
    
    type = TouchType::None;
    timestamp = 0;
#if TARGET_OS_IPHONE
    touch = nil;
#endif /* TARGET_OS_IPHONE */
    x = 0;
    y = 0;
    tapCount = 0;
}

void BGE::Input::destroy() {
    type = TouchType::None;
#if TARGET_OS_IPHONE
    touch = nil;
#endif /* TARGET_OS_IPHONE */
    
    handle_ = InputHandle();
}
