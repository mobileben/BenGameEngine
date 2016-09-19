//
//  InputTouchComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputTouchComponent_h
#define InputTouchComponent_h

#include <stdio.h>
#include "Component.h"

namespace BGE {
    class InputTouchComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId         typeId_;
        static uint32_t         bitmask_;
        
        InputTouchComponent() {}
        ~InputTouchComponent() {}
    };
}

#endif /* InputTouchComponent_h */
