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

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class SpriteRenderComponent : public BGE::RenderComponent
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<SpriteRenderComponent> create(uint64_t componentId);
        static std::shared_ptr<SpriteRenderComponent> create(uint64_t componentId, std::string name);
        
        SpriteRenderComponent(struct private_key const& key, uint64_t componentId);
        SpriteRenderComponent(struct private_key const& key, uint64_t componentId, std::string name);
        virtual ~SpriteRenderComponent() {}

        std::shared_ptr<TextureBase> getTexture();
        
    protected:
        SpriteRenderComponent(uint32_t componentId);
        SpriteRenderComponent(uint32_t componentId, std::string name);
        
        void materialsUpdated();
        
        BGEVertexTex* const getVertices() { return vertices_; }
        
    private:
        friend BGE::ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;
        
        static const uint32_t NumVertices = 4;
        
        BGEVertexTex vertices_[NumVertices];
        
        void updateLocalBoundsAndVertices();
    };
}

#endif /* BGESpriteRenderComponent_h */
