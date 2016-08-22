//
//  BoundingBoxComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BoundingBoxComponent_h
#define BoundingBoxComponent_h

#include <stdio.h>
#include "Component.h"
#include "MathTypes.h"

namespace BGE {
    class BoundingBoxComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        float x;
        float y;
        float width;
        float height;
        
        float aabbMinX;
        float aabbMinY;
        float aabbMaxX;
        float aabbMaxY;
        
        BoundingBoxComponent();
        ~BoundingBoxComponent() {}
        
        void computeAABB(Matrix4 &transform);
    };
}

#endif /* BoundingBoxComponent_h */
