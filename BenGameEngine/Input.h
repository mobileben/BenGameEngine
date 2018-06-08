//
//  Input.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Input_h
#define Input_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#endif /* TARGET_OS_IPHONE */
#include <stdio.h>
#include "Object.h"
#include "Handle.h"
#include <functional>

namespace BGE {
    class InputService;
    class Input;
    
    using InputHandler = std::function<void(Input *)>;
    
    enum class TouchType {
        None,
        Down,
        Up,
        Move,
        Cancel
    };
    
    class Input : public Object {
    public:
        TouchType       type;

        double          timestamp;
        
#if TARGET_OS_IPHONE
        UITouch         *touch;
#endif /* TARGET_OS_IPHONE */
        
        float           x;
        float           y;
        uint32_t        tapCount;
        
        Input();
        ~Input() {}
        
        void initialize(InputHandle handle);
        void destroy();
        
        inline InputHandle getHandle() const { return handle_; }
        
        bool operator <(const Input &rhs) const { return timestamp < rhs.timestamp; }
        
    protected:
        friend InputService;
        
    private:
        InputHandle     handle_;
    };
}

#endif /* Input_h */
