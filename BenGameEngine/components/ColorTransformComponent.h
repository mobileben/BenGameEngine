//
//  ColorTransformComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ColorTransformComponent_h
#define ColorTransformComponent_h

#include <stdio.h>
#include "Component.h"
#include "MathTypes.h"

namespace BGE {
    class ColorTransformComponent : public Component {
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        ColorTransformComponent();
        ~ColorTransformComponent() {}
        
        ColorTransform transform;
    };
}

#endif /* ColorTransformComponent_h */
