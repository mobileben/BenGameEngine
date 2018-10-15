//
//  ColorMatrixComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ColorMatrixComponent_h
#define ColorMatrixComponent_h

#include <stdio.h>
#include "Component.h"
#include "MathTypes.h"

namespace BGE {
    class ColorMatrixComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        ColorMatrixComponent();
        ~ColorMatrixComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            Component::initialize(handle, spaceHandle);
            
            ColorMatrixMakeIdentity(matrix);
        }

        ColorMatrix matrix;
    };
}

#endif /* ColorMatrixComponent_h */
