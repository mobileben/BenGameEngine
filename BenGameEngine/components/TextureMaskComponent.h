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
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        TextureMaskComponent();
        ~TextureMaskComponent() {}
        
        TextureHandle getTextureHandle();
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
