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
        static const int32_t AnimPlayForever = -1;
        
        static std::shared_ptr<ColorMatrixComponent> create(uint64_t componentId);
        static std::shared_ptr<ColorMatrixComponent> create(uint64_t componentId, std::string name);
        
        ColorMatrixComponent(struct private_key const& key, uint64_t componentId);
        ColorMatrixComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        ~ColorMatrixComponent() {}

        ColorMatrix matrix;
    };
}

#endif /* ColorMatrixComponent_h */
