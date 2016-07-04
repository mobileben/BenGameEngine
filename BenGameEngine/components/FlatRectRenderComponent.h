//
//  FlatRectRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFlatRectRenderComponent_h
#define BGEFlatRectRenderComponent_h

#include <stdio.h>
#include "RenderService.h"
#include "RenderComponent.h"

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class FlatRectRenderComponent : public RenderComponent
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;

    public:
        static std::shared_ptr<FlatRectRenderComponent> create(ObjectId componentId);
        
        FlatRectRenderComponent(struct private_key const& key, ObjectId componentId);
        ~FlatRectRenderComponent() {}

        void setWidth(float width);
        void setHeight(float height);
        void setWidthHeight(Vector2 &wh);
        
    protected:
        Vertex* const getVertices() { return vertices_; }
        
        void materialsUpdated();
        
    private:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;

        static const uint32_t NumVertices = 4;
        
        Vertex vertices_[NumVertices];
        
        void updateLocalBoundsAndVertices(Vector2& wh);
    };
}

#endif /* BGEFlatRectRenderComponent_h */
