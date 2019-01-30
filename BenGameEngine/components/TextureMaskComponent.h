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
    class RenderService;
    class RenderServiceOpenGLES2;

    class TextureMaskComponent : public RenderComponent
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        TextureMaskComponent();
        ~TextureMaskComponent() {}
        
        TextureHandle getTextureHandle();
        void setTextureMaskReference(TextureMaskReference *maskRef);
        void setTextureMaskReference(const TextureMaskReference &maskRef);
        
        void setWidth(float width);
        void setHeight(float height);

        void reposition(Vector2 position);
        void resetPosition();
        void resize(Vector2 size);
        void resetSize();

    protected:
        VertexTex* getVertices() { return vertices_; }
        
        void materialsUpdated();
        
    private:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;

        static const uint32_t NumVertices = 4;
        
        Vector2 originalSize_;
        VertexTex vertices_[NumVertices];
        
        void setWidthHeight(Vector2 &wh);
        void updateLocalBoundsAndVertices();
        void updateLocalBoundsAndVertices(Vector2& wh);
    };
}

#endif /* TextureMaskComponent_h */
