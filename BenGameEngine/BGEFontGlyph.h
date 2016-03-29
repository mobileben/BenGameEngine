//
//  BGEFontGlyph.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGEFontGlyph_h
#define BGEFontGlyph_h

#include <stdio.h>
#include <memory>
#include "BGETexture.h"

typedef struct {
    int32_t offsetX;
    int32_t offsetY;
    int32_t advance;
} BGEFontGlyphDef;

class BGEFont;

class BGEFontGlyph
{
public:
    // TODO: Determine if this should be a std::shared_ptr
    BGEFontGlyph(BGEFont *font, std::shared_ptr<BGETexture> texture, int32_t offsetX, int32_t offsetY, int32_t advance);
    virtual ~BGEFontGlyph() {}
    
    int32_t getOffsetX() const { return offsetX_; }
    int32_t getAdvance() const { return advance_; }
    int32_t getOffsetY() const;
    
    std::shared_ptr<BGETexture> getTexture() const { return texture_; }
    
private:
    int32_t offsetX_;
    int32_t offsetY_;
    int32_t offsetYInverted_;
    int32_t advance_;
    
    // TODO: Make weak?
    std::shared_ptr<BGETexture> texture_;
};

#endif /* BGEFontGlyph_h */
