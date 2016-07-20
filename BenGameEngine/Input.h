//
//  Input.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Input_h
#define Input_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <stdio.h>
#include "Handle.h"

namespace BGE {
    struct InputTag {};
    using InputHandle = Handle<InputTag>;
    
    enum class InputType {
        None,
        Touch
    };
    
    enum class TouchType {
        None,
        Start,
        End,
        Move,
        Cancel
    };
    
    class Input {
        InputType       inputType;
        NSTimeInterval  timestamp;
        
        union {
            struct {
                TouchType   type;
                UITouch     *touch;
                float       x;
                float       y;
                uint32_t    tapCount;
            } touch;
        } data;
    };
}

#endif /* Input_h */
