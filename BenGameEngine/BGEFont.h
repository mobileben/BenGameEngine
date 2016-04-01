//
//  BGEFont.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFont_h
#define BGEFont_h

#include <stdio.h>
#include <string>
#include <map>
#include "BGEFontGlyph.h"
#include "BGETextureAtlas.h"
#include "BGEMathTypes.h"

enum class BGEFontHorizontalAlignment {
    Left,
    Center,
    Right
};

enum class BGEFontVerticalAlignment {
    Top,
    Center,
    Bottom,
    Baseline
};

class BGEFont
{
public:
    BGEFont(std::string name, uint32_t pixelSize, std::string filename);
    virtual ~BGEFont();
    
    uint32_t getGlyphW() const { return glyphW_; }
    uint32_t getGlyphH() const { return glyphH_; }
    int32_t getBaseline() const { return baseline_; }
    
    std::shared_ptr<BGEFontGlyph> glyphForExtendedASCII(uint16_t code);
    
    bool hasKerning() const { return hasKerning_; }
    int32_t kerningForPair(uint16_t prev, uint16_t curr);
    uint32_t getStringWidth(std::string str, bool minimum=true);
    uint32_t getHeight() const;
    
    virtual void drawString(std::string str, BGEVector2 &position, BGEVector4 &color, BGEFontHorizontalAlignment horizAlignment=BGEFontHorizontalAlignment::Center, BGEFontVerticalAlignment vertAlignment=BGEFontVerticalAlignment::Center, bool minimum=true) =0;
    
protected:
    std::string name_;
    uint32_t pixelSize_;
    
    uint32_t glyphW_;   // Max width of glyphs
    uint32_t glyphH_;   // Max height of glyphs
    int32_t baseline_;   // Baseline Y position. Hang is below (negative). Bearing Y is above (positive).
    
    bool hasKerning_;
    
    std::shared_ptr<BGETextureAtlas> textureAtlas_;
    std::map<uint16_t, std::shared_ptr<BGEFontGlyph>> glyphs_;
    std::map<std::pair<uint16_t, uint16_t>, int32_t> kerning_;
};

#endif /* BGEFont_h */
