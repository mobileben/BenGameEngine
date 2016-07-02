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
    private:
        struct private_key {};
        
    public:
        static const int32_t AnimPlayForever = -1;
        
        static std::shared_ptr<ColorTransformComponent> create(uint64_t componentId);
        static std::shared_ptr<ColorTransformComponent> create(uint64_t componentId, std::string name);
        
        ColorTransformComponent(struct private_key const& key, uint64_t componentId);
        ColorTransformComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        ~ColorTransformComponent() {}
        
        ColorTransform transform;
    };
}

#endif /* ColorTransformComponent_h */
