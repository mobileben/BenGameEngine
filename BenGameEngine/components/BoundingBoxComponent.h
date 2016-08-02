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
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;
        
    public:
        float x;
        float y;
        float width;
        float height;
        
        float aabbMinX;
        float aabbMinY;
        float aabbMaxX;
        float aabbMaxY;
        
        static std::shared_ptr<BoundingBoxComponent> create(ObjectId componentId);
        
        BoundingBoxComponent(struct private_key const& key, ObjectId componentId);
        ~BoundingBoxComponent() {}
        
        void computeAABB(Matrix4 &transform);
    };
}

#endif /* BoundingBoxComponent_h */
