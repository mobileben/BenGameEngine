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
    
    class FlatRectRenderComponent : public BGE::RenderComponent
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<FlatRectRenderComponent> create(uint64_t componentId);
        static std::shared_ptr<FlatRectRenderComponent> create(uint64_t componentId, std::string name);
        
        FlatRectRenderComponent(struct private_key const& key, uint64_t componentId);
        FlatRectRenderComponent(struct private_key const& key, uint64_t componentId, std::string name);
        virtual ~FlatRectRenderComponent() {}

        void setWidth(float width);
        void setHeight(float height);
        void setWidthHeight(BGEVector2 &wh);
        
    protected:
        FlatRectRenderComponent(uint32_t componentId);
        FlatRectRenderComponent(uint32_t componentId, std::string name);
        
        BGEVertex* const getVertices() { return vertices_; }
        
        void materialsUpdated();
        
    private:
        friend BGE::ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;
        
        static const uint32_t NumVertices = 4;
        
        BGEVertex vertices_[NumVertices];
        
        void updateLocalBoundsAndVertices(BGEVector2& wh);
    };
}

#endif /* BGEFlatRectRenderComponent_h */
