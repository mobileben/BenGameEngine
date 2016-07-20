//
//  TextureMaskComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/13/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef TextureMaskComponent_h
#define TextureMaskComponent_h

#include <stdio.h>
#include <memory>
#include "RenderComponent.h"
#include "RenderService.h"
#include "GraphicFormats.h"

namespace BGE {
    class TextureMaskComponent : public RenderComponent
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;
        
    public:
        static std::shared_ptr<TextureMaskComponent> create(ObjectId componentId);
        
        TextureMaskComponent(struct private_key const& key, ObjectId componentId);
        ~TextureMaskComponent() {}
        
        std::shared_ptr<TextureBase> getTexture();
        void setTextureMaskReference(TextureMaskReference *maskRef);
        void setTextureMaskReference(const TextureMaskReference &maskRef);
        
        
    protected:
        Vertex* const getVertices() { return vertices_; }
        
        void materialsUpdated();
        
    private:
        static const uint32_t NumVertices = 4;
        
        Vertex vertices_[NumVertices];
        
        void updateLocalBoundsAndVertices();
    };
}

#endif /* TextureMaskComponent_h */
