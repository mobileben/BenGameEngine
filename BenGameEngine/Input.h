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

namespace BGE {
    struct InputTag {};
    using InputHandle = Handle<InputTag>;
    
    class InputService;
    
    enum class TouchType {
        None,
        Down,
        Up,
        Move,
        Cancel
    };
    
    class Input : public Object {
    public:
        Input(ObjectId matId);
        ~Input() {}
        
        void initialize(InputHandle handle, ObjectId objectId);
        
        TouchType       type;
        InputHandle getHandle() const { return handle_; }

        NSTimeInterval  timestamp;
        
        UITouch         *touch;
        float           x;
        float           y;
        uint32_t        tapCount;
        
    protected:
        friend InputService;
        
    private:
        InputHandle     handle_;
    };
}

#endif /* Input_h */
