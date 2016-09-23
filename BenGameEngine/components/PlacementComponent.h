//
//  PlacementComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef PlacementComponent_h
#define PlacementComponent_h

#include <stdio.h>
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {
    class RenderServiceOpenGLES2;
    class GameObject;
    
    class PlacementComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        PlacementComponent();
        ~PlacementComponent() {}
        
        void setPlacementReference(PlacementReference *placementRef);
        void setPlacementReference(const PlacementReference &placementRef);
        
        inline float getWidth() const { return width_; }
        inline float getHeight() const { return height_; }
        
        void setWidth(float width);
        void setHeight(float height);
        void setWidthHeight(Vector2 &wh);
        
        void addChild(GameObjectHandle handle);
        void addChild(GameObject *object);
        
    private:
        float                   width_;
        float                   height_;
    };
}


#endif /* PlacementComponent_h */
