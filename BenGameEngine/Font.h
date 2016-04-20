//
//  Font.h
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
#include "FontGlyph.h"
#include "TextureAtlas.h"
#include "BGEMathTypes.h"

namespace BGE {
    enum class FontHorizontalAlignment {
        Left,
        Center,
        Right
    };
    
    enum class FontVerticalAlignment {
        Top,
        Center,
        Bottom,
        Baseline
    };
    
    class Font
    {
    public:
        Font(std::string name, uint32_t pixelSize, std::string filename);
        virtual ~Font();
        
        uint32_t getGlyphW() const { return glyphW_; }
        uint32_t getGlyphH() const { return glyphH_; }
        int32_t getBaseline() const { return baseline_; }
        
        std::shared_ptr<FontGlyph> glyphForExtendedASCII(uint16_t code);
        
        bool hasKerning() const { return hasKerning_; }
        int32_t kerningForPair(uint16_t prev, uint16_t curr);
        uint32_t getStringWidth(std::string str, bool minimum=true);
        uint32_t getHeight() const;
        
        virtual void drawString(std::string str, BGEVector2 &position, BGEVector4 &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true) =0;
        
    protected:
        std::string name_;
        uint32_t pixelSize_;
        
        uint32_t glyphW_;   // Max width of glyphs
        uint32_t glyphH_;   // Max height of glyphs
        int32_t baseline_;   // Baseline Y position. Hang is below (negative). Bearing Y is above (positive).
        
        bool hasKerning_;
        
        std::shared_ptr<BGE::TextureAtlas> textureAtlas_;
        std::map<uint16_t, std::shared_ptr<FontGlyph>> glyphs_;
        std::map<std::pair<uint16_t, uint16_t>, int32_t> kerning_;
    };
}

#endif /* BGEFont_h */
