//
//  SpriteRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpriteRenderComponent_h
#define BGESpriteRenderComponent_h

#include <stdio.h>
#include "RenderService.h"
#include "RenderComponent.h"
#include "GraphicFormats.h"

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class SpriteRenderComponent : public RenderComponent
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;

    public:
        static std::shared_ptr<SpriteRenderComponent> create(ObjectId componentId);
        
        SpriteRenderComponent(struct private_key const& key, ObjectId componentId);
        ~SpriteRenderComponent() {}

        TextureHandle getTextureHandle();
        void setTextureReference(TextureReference *texRef);
        void setTextureReference(const TextureReference &texRef);
        
    protected:
        void materialsUpdated();
        
        VertexTex* const getVertices() { return vertices_; }
        
    private:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;

        static const uint32_t NumVertices = 4;
        
        VertexTex vertices_[NumVertices];
        
        void updateLocalBoundsAndVertices();
    };
}

#endif /* BGESpriteRenderComponent_h */
