//
//  InputTouchComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputTouchComponent_h
#define InputTouchComponent_h

#import <UIKit/UIKit.h>

#include <stdio.h>
#include "Component.h"

namespace BGE {
    class InputTouchComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;

        UITouch *touch;
        
        InputTouchComponent() : touch(nil) {}
        ~InputTouchComponent() {}

        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
    };
}

#endif /* InputTouchComponent_h */
