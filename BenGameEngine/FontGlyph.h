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
        // TODO: Determine if this should be a std::shared_ptr
        FontGlyph(Font *font, std::shared_ptr<BGE::Texture> texture, int32_t offsetX, int32_t offsetY, int32_t advance);
        virtual ~FontGlyph() {}
        
        int32_t getOffsetX() const { return offsetX_; }
        int32_t getAdvance() const { return advance_; }
        int32_t getOffsetY() const;
        
        std::shared_ptr<BGE::Texture> getTexture() const { return texture_; }
        
    private:
        int32_t offsetX_;
        int32_t offsetY_;
        int32_t offsetYInverted_;
        int32_t advance_;
        
        // TODO: Make weak?
        std::shared_ptr<BGE::Texture> texture_;
    };
}

#endif /* BGEFontGlyph_h */
