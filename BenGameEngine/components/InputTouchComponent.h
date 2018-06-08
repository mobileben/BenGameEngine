//
//  InputTouchComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputTouchComponent_h
#define InputTouchComponent_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif /* TARGET_OS_IPHONE */

#include <stdio.h>
#include "Component.h"

namespace BGE {
    class InputTouchComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;

#if TARGET_OS_IPHONE
        UITouch *touch;
#endif /* TARGET_OS_IPHONE */
        
        InputTouchComponent() {
#if TARGET_OS_IPHONE
            touch = nil;
#endif /* TARGET_OS_IPHONE */
        }
        ~InputTouchComponent() {}

        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
    };
}

#endif /* InputTouchComponent_h */
