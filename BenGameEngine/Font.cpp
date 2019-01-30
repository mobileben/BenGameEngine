//
//  Font.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Font.h"
#include "Game.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "RenderServiceOpenGLES2.h"
#include "ShaderServiceOpenGLES2.h"
#include "ft2build.h"

#include FT_FREETYPE_H

// InitialSupportedCharacterOffset is also ASCII for the space character
static const int InitialSupportedCharacterOffset = 32;
static const int NumSupportedCharacters = 256 - InitialSupportedCharacterOffset;   // Support extended ASCCI, ignore control characters (0-31)

const std::string BGE::Font::ErrorDomain = "Font";

BGE::Font::Font() : NamedObject(), pixelSize_(0), handle_(FontHandle()), status_(FontStatus::Invalid), hasKerning_(false), textureAtlasHandle_(TextureAtlasHandle()) {
}

BGE::Font::Font(ObjectId fontId) : NamedObject(fontId), pixelSize_(0), handle_(FontHandle()), status_(FontStatus::Invalid), hasKerning_(false), textureAtlasHandle_(TextureAtlasHandle()) {
}

BGE::Font::Font(const std::string& name, uint32_t pixelSize) : NamedObject(0, name), pixelSize_(pixelSize), handle_(FontHandle()), status_(FontStatus::Invalid), hasKerning_(false), textureAtlasHandle_(TextureAtlasHandle()) {
}

void BGE::Font::initialize(FontHandle handle, const std::string& name, uint32_t pixelSize) {
    setName(name);
    handle_ = handle;
    pixelSize_ = pixelSize;
    status_ = FontStatus::Invalid;
    glyphW_ = 0;
    glyphH_ = 0;
    baseline_ = 0;
    hasKerning_ = false;
    Game::getInstance()->getTextureService()->removeTextureAtlas(getHandle(), textureAtlasHandle_);
    
    textureAtlasHandle_ = TextureAtlasHandle();
}

void BGE::Font::destroy() {
    status_ = FontStatus::Invalid;

    Game::getInstance()->getTextureService()->removeTextureAtlas(handle_, textureAtlasHandle_);
    
    family_.clear();
    style_.clear();
    glyphs_.clear();
    kerning_.clear();
    
    textureAtlasHandle_ = TextureAtlasHandle();
}

#ifdef NOT_YET
std::shared_ptr<BGE::FontGlyph> BGE::Font::glyphForExtendedASCII(uint16_t code) {
    return glyphs_[code];
}
#endif

std::string BGE::Font::getNameAsKey() const {
    return FontService::fontAsKey(getName(), pixelSize_);
}

int32_t BGE::Font::kerningForPair(uint16_t prev, uint16_t curr) {
    int32_t kerning = 0;
    
    if (prev && curr) {
        std::map<std::pair<uint16_t, uint16_t>, int32_t>::iterator it = kerning_.find(std::make_pair(prev, curr));
        
        if (it != kerning_.end()) {
            kerning = it->second;
        }
    }
    
    return kerning;
}

uint32_t BGE::Font::getStringWidth(std::string str, __attribute__ ((unused)) bool minimum) {
    int32_t width = 0;
    
    if (status_ == FontStatus::Valid) {
        const char *chars = str.c_str();
        uint16_t code;
        size_t length = str.length();
        uint16_t prev = 0;

        for (auto i=0u;i<length;i++) {
            code = chars[i];

            auto glyph = glyphs_.find(code);

            if (glyph != glyphs_.end()) {
                if (hasKerning() && prev) {
                    width += kerningForPair(prev, code);
                }

                width += glyph->second.getAdvance();
            }

            prev = code;
        }
        
        if (width < 0) {
            width = 0;
        }
    }
    
    return (uint32_t)width;
}

uint32_t BGE::Font::getHeight() const {
    if (status_ == FontStatus::Valid) {
        return getGlyphH();
    } else {
        return 0;
    }
}

std::pair<BGE::FontHandle, std::shared_ptr<BGE::Error>> BGE::Font::load(std::string filename, uint32_t faceIndex)  {
    FT_Face face = NULL;
    FT_Error error = 0;
    FT_Library library;
    error = FT_Init_FreeType(&library);
    std::shared_ptr<Error> bgeError;
    FontHandle fontHandle;

    if (!error) {
        error = FT_New_Face(library, filename.c_str(), faceIndex, &face);
        
        if (!error) {
            error = FT_Set_Pixel_Sizes(face, 0, pixelSize_);
            
            if (!error) {
                FT_Glyph_Metrics metrics[NumSupportedCharacters];
                FT_Bitmap bitmaps[NumSupportedCharacters];
                long maxBearing = 0;
                long minHang = 0;
                long top;
                long bottom;
                long width;
                long height;
                long maxWidth = 0;
                
                // Go through ASCII. Include extended ASCII. Ignore control characters
                for (int i=0;i<NumSupportedCharacters;i++) {
                    error = FT_Load_Char( face, i + InitialSupportedCharacterOffset, FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT|FT_LOAD_TARGET_LIGHT );
                    
                    if (!error) {
                        FT_Glyph_Metrics *currMetrics = &face->glyph->metrics;
                        unsigned char *newBuffer;
                        
                        metrics[i] = *currMetrics;
                        bitmaps[i] = face->glyph->bitmap;
                        
                        if (bitmaps[i].buffer) {
                            newBuffer = (unsigned char *)malloc(bitmaps[i].pitch * bitmaps[i].rows);
                            memcpy(newBuffer, bitmaps[i].buffer, bitmaps[i].pitch * bitmaps[i].rows);
                            bitmaps[i].buffer = newBuffer;
                        }
                        
                        width = currMetrics->width >> 6;
                        height = currMetrics->height >> 6;
                        top = currMetrics->horiBearingY >> 6;
                        bottom = top - height;
                        
                        if (width > maxWidth) {
                            maxWidth = width;
                        }
                        
                        if (top > maxBearing) {
                            maxBearing = top;
                        }
                        
                        if (bottom < minHang) {
                            minHang = bottom;
                        }
                    } else {
                        break;
                    }
                }
                
                int glyphW = (int) maxWidth;
                int glyphH = (int) (maxBearing - minHang);
                int maxCol = 16;
                int maxRow = NumSupportedCharacters / maxCol;
                int atlasW = glyphW * maxCol;
                int atlasH = glyphH * maxRow;
                int atlasSpan = glyphW * maxCol;
                
                glyphW_ = glyphW;
                glyphH_ = glyphH;
                baseline_ = (int) maxBearing;
                
                atlasSpan = atlasW;
                
                unsigned char *atlasBuffer = (unsigned char *)malloc(atlasW * atlasH);
                
                // Generate one larger texture for the atlas
                if (atlasBuffer) {
                    int index;
                    int x = 0, y = 0;
                    unsigned char *dest;
                    unsigned char *currBuffer;
                    SubTextureDef subTexDef;
                    FontGlyphDef glyphDefs[NumSupportedCharacters];
                    std::vector<SubTextureDef> subTexDefs;
                    std::string fontKeyBase = FontService::fontAsKey(getName(), pixelSize_) + "_";
                    
                    memset(atlasBuffer, 0, atlasW * atlasH);
                    
                    // 16 columns, 14 rows
                    for (int col=0;col<maxCol;col++) {
                        x = col * glyphW;
                        
                        for (int row=0;row<maxRow;row++) {
                            index = row * maxCol + col;
                            
                            y = row * glyphH;
                            
                            FT_Bitmap *currBitmap = &bitmaps[index];
                            
                            currBuffer = currBitmap->buffer;
                            
                            // We want the key to use the proper extended ASCII value, so add InitialSupportedCharacterOffset
                            subTexDef.name = fontKeyBase + std::to_string(index + InitialSupportedCharacterOffset);
                            subTexDef.x = x;
                            subTexDef.y = y;
                            
                            glyphDefs[index].offsetX = (int32_t) (metrics[index].horiBearingX >> 6);
                            glyphDefs[index].offsetY = (int32_t) (maxBearing - (metrics[index].horiBearingY >> 6));
                            glyphDefs[index].advance = (int32_t) (metrics[index].horiAdvance >> 6);
                            
                            if (currBuffer) {
                                subTexDef.width = currBitmap->width;
                                subTexDef.height = currBitmap->rows;
                                
                                dest = &atlasBuffer[y * atlasSpan + x];
                                
                                for (auto line=0u;line<currBitmap->rows;line++) {
                                    memcpy(&dest[line * atlasSpan], &currBuffer[line * currBitmap->pitch], currBitmap->width);
                                }
                            } else {
                                subTexDef.width = 0;
                                subTexDef.height = 0;
                            }
                            
                            subTexDef.rotated = false;
                            
                            subTexDefs.push_back(subTexDef);
                        }
                    }
                    
                    // Build kerning information
                    hasKerning_ = FT_HAS_KERNING(face);
                    
                    if (hasKerning_) {
                        // prev is "previous character"
                        // curr is "current character"
                        // prev and curr are used as the kerning pair
                        uint16_t prevGlyphCode;
                        uint16_t prevGlyphIndex;
                        uint16_t currGlyphCode;
                        uint16_t currGlyphIndex;
                        FT_Vector delta;
                        int32_t kerningAmount;
                        
                        for (int prev=0;prev<NumSupportedCharacters;prev++) {
                            prevGlyphCode = prev + InitialSupportedCharacterOffset;
                            prevGlyphIndex = FT_Get_Char_Index(face, prevGlyphCode);
                            
                            if (prevGlyphIndex) {
                                for (int curr=0;curr<NumSupportedCharacters;curr++) {
                                    currGlyphCode = curr + InitialSupportedCharacterOffset;
                                    currGlyphIndex = FT_Get_Char_Index(face, currGlyphCode);
                                    
                                    if (currGlyphIndex) {
                                        FT_Get_Kerning(face, prevGlyphIndex, currGlyphIndex, FT_KERNING_DEFAULT, &delta);
                                        
                                        kerningAmount = (int32_t) (delta.x >> 6);
                                        
                                        if (kerningAmount != 0) {
                                            kerning_[std::make_pair(prevGlyphCode, currGlyphCode)] = kerningAmount;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    TextureAtlas *atlas;
                    std::tie(atlas, bgeError) = Game::getInstance()->getTextureService()->createTextureAtlasFromBuffer(getHandle(), FontService::fontAsKey(getName(), pixelSize_), atlasBuffer, TextureFormat::Alpha, atlasW, atlasH, subTexDefs);
                    if (atlas) {
                        textureAtlasHandle_ = atlas->getHandle();
                        glyphs_.clear();

                        // Our space is used whenever we don't have a match
                        uint16_t code;
                        std::string key = fontKeyBase + std::to_string(InitialSupportedCharacterOffset);
                        auto subTexHandle = atlas->getSubTextureHandle(key);
                        FontGlyph space = FontGlyph(this, subTexHandle, glyphDefs[0].offsetX, glyphDefs[0].offsetY, glyphDefs[0].advance);

                        glyphs_[InitialSupportedCharacterOffset] = space;

                        // Space is already done
                        for (int i=0;i<NumSupportedCharacters + 1;i++) {
                            code = i + InitialSupportedCharacterOffset;
                            key = fontKeyBase + std::to_string(code);
                            subTexHandle = atlas->getSubTextureHandle(key);

                            if (!subTexHandle.isNull()) {
                                auto glyph = FontGlyph(this, subTexHandle, glyphDefs[i].offsetX, glyphDefs[i].offsetY, glyphDefs[i].advance);

                                glyphs_[code] = glyph;
                            } else {
                                // Replace with SPACE
                                glyphs_[code] = space;
                            }
                        }
                    } else {
                        textureAtlasHandle_ = TextureAtlasHandle();
                    }

                    for (int i=0;i<NumSupportedCharacters;i++) {
                        if (bitmaps[i].buffer) {
                            free(bitmaps[i].buffer);
                        }
                    }

                    free(atlasBuffer);
                    status_ = FontStatus::Valid;
                    fontHandle = handle_;
                } else {
                    bgeError = std::make_shared<Error>(Font::ErrorDomain, FontErrorAllocation);
                }
            } else {
                bgeError = std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType);
            }
            FT_Done_Face(face);
        } else  {
            bgeError = std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType);
        }
        FT_Done_FreeType(library);
    } else {
        bgeError = std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType);
    }
    return std::make_pair(fontHandle, bgeError);
}

