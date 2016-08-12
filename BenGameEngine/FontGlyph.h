//
//  FontGlyph.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontGlyph_h
#define BGEFontGlyph_h

#include <stdio.h>
#include <memory>
#include "Texture.h"

typedef struct {
    int32_t offsetX;
    int32_t offsetY;
    int32_t advance;
} FontGlyphDef;

namespace BGE {
    class Font;
    
    class FontGlyph
    {
    public:
        FontGlyph();
        FontGlyph(Font *font, TextureHandle textureHandle, int32_t offsetX, int32_t offsetY, int32_t advance);
        virtual ~FontGlyph() {}
        
        int32_t getOffsetX() const { return offsetX_; }
        int32_t getAdvance() const { return advance_; }
        int32_t getOffsetY() const;
        
        TextureHandle getTextureHandle() const { return textureHandle_; }
        
    private:
        int32_t offsetX_;
        int32_t offsetY_;
        int32_t offsetYInverted_;
        int32_t advance_;
        
        TextureHandle   textureHandle_;
    };
}

#endif /* BGEFontGlyph_h */
