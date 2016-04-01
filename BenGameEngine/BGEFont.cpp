//
//  BGEFont.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEFont.h"
#include "BGEGame.h"

BGEFont::BGEFont(std::string name, uint32_t pixelSize, std::string filename) : name_(name), pixelSize_(pixelSize), textureAtlas_(nullptr), hasKerning_(false) {
    
}

BGEFont::~BGEFont() {
    BGEGame::getInstance()->getTextureService()->removeTexture(textureAtlas_);
}

std::shared_ptr<BGEFontGlyph> BGEFont::glyphForExtendedASCII(uint16_t code) {
    return glyphs_[code];
}

int32_t BGEFont::kerningForPair(uint16_t prev, uint16_t curr) {
    int32_t kerning = 0;
    
    if (prev && curr) {
        std::map<std::pair<uint16_t, uint16_t>, int32_t>::iterator it = kerning_.find(std::make_pair(prev, curr));
        
        if (it != kerning_.end()) {
            kerning = it->second;
        }
    }
    
    return kerning;
}

uint32_t BGEFont::getStringWidth(std::string str, bool minimum) {
    int32_t width = 0;
    const char *chars = str.c_str();
    std::shared_ptr<BGEFontGlyph> glyph;
    uint16_t code;
    size_t length = str.length();
    
    if (length == 1) {
        if (!minimum) {
            width = glyph->getAdvance();
        } else {
            width = glyph->getTexture()->getWidth();
        }
    } else {
        uint16_t prev = 0;
        
        for (int i=0;i<length;i++) {
            code = chars[i];
            
            glyph = glyphs_[code];
            
            if (glyph) {
                if (i==0) {
                    if (!minimum) {
                        width += glyph->getAdvance();
                    } else {
                        width += glyph->getAdvance() - glyph->getOffsetX();
                    }
                } else if (i==(length-1)) {
                    if (!minimum) {
                        width += glyph->getAdvance();
                    } else {
                        width += glyph->getOffsetX() + glyph->getTexture()->getWidth();
                    }
                } else {
                    width += glyph->getAdvance();
                }
            }
            
            if (hasKerning() && prev) {
                width += kerningForPair(prev, code);
            }
            
            prev = code;
        }
    }
    
    if (width < 0) {
        width = 0;
    }
    
    return (uint32_t)width;
}

uint32_t BGEFont::getHeight() const {
    return getGlyphH();
}
