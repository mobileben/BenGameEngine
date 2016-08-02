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
#include "Object.h"
#include "Handle.h"
#include <functional>

namespace BGE {
    struct InputTag {};
    using InputHandle = Handle<InputTag>;

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
        InputHandle getHandle() const { return handle_; }
        
        NSTimeInterval  timestamp;
        
        UITouch         *touch;
        float           x;
        float           y;
        uint32_t        tapCount;
        
        Input(ObjectId matId);
        ~Input() {}
        
        void initialize(InputHandle handle, ObjectId objectId);
        
        bool operator <(const Input &rhs) const { return timestamp < rhs.timestamp; }
        
    protected:
        friend InputService;
        
    private:
        InputHandle     handle_;
    };
}

#endif /* Input_h */
