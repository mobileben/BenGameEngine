//
//  MaskComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef MaskComponent_h
#define MaskComponent_h

#include <stdio.h>
#include <memory>
#include "RenderComponent.h"
#include "RenderService.h"
#include "GraphicFormats.h"

namespace BGE {
    class RenderServiceOpenGLES2;

    class MaskComponent : public RenderComponent
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        MaskComponent();
        ~MaskComponent() {}
        
        void setMaskReference(MaskReference *maskRef);
        void setMaskReference(const MaskReference &maskRef);
        
        void setWidth(float width);
        void setHeight(float height);

        void reposition(Vector2 position);
        void resetPosition();
        void resize(Vector2 size);
        void resetSize();
        
    protected:
        Vertex* const getVertices() { return vertices_; }
        
        void materialsUpdated();

    private:
        friend RenderServiceOpenGLES2;

        static const uint32_t NumVertices = 4;
        
        Vector2 originalSize_;
        Vertex vertices_[NumVertices];
        
        void setWidthHeight(Vector2 &wh);
        void updateLocalBoundsAndVertices(Vector2& wh);
    };
}

#endif /* MaskComponent_h */
