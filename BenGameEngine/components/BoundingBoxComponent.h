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
        static ComponentTypeId  typeId_;
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
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        void destroy() final;

        void computeAABB(Matrix4 &transform);
        void computeAABB(Matrix4 &transform, Vector2 scale);
        
    private:
        void computeAABB(Matrix4 &transform, float w, float h);
    };
}

#endif /* BoundingBoxComponent_h */
