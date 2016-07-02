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
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<ColorMatrixComponent> create(ObjectId componentId);
        
        ColorMatrixComponent(struct private_key const& key, ObjectId componentId);
        ~ColorMatrixComponent() {}

        ColorMatrix matrix;
    };
}

#endif /* ColorMatrixComponent_h */
