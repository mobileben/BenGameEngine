//
//  FontOpenGLES2.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Game.h"
#include "FontOpenGLES2.h"
#include "TextureOpenGLES2.h"
#include "RenderServiceOpenGLES2.h"
#include "ShaderServiceOpenGLES2.h"
#include "ft2build.h"
#include FT_FREETYPE_H

// InitialSupportedCharacterOffset is also ASCII for the space character
static const int InitialSupportedCharacterOffset = 32;
static const int NumSupportedCharacters = 256 - InitialSupportedCharacterOffset;   // Support extended ASCCI, ignore control characters (0-31)

BGE::FontOpenGLES2::FontOpenGLES2(std::string name, uint32_t pixelSize) : BGE::Font(name, pixelSize) {
}

void BGE::FontOpenGLES2::load(std::string filename, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback)  {
    FT_Face face = NULL;
    FT_Error error = 0;
    
    error = FT_New_Face(BGE::Game::getInstance()->getFontService()->getFreetypeLibrary(), filename.c_str(), 0, &face);
    
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
                error = FT_Load_Char( face, i + InitialSupportedCharacterOffset, FT_LOAD_RENDER );
                
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
            int glyphSize = glyphW * glyphH;
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
                BGESubTextureDef subTexDef;
                FontGlyphDef glyphDefs[NumSupportedCharacters];
                std::map<std::string, BGESubTextureDef> subTexDefs;
                std::string fontKeyBase = FontService::fontAsKey(name_, pixelSize_) + "_";
                
                memset(atlasBuffer, 0, atlasW * atlasH);
                
                // 16 columns, 14 rows
                for (int col=0;col<maxCol;col++) {
                    x = col * glyphW;
                    
                    for (int row=0;row<maxRow;row++) {
                        index = row * maxCol + col;
                        
                        y = row * glyphH;
                        
                        FT_Bitmap *currBitmap = &bitmaps[index];
                        
                        currBuffer = currBitmap->buffer;
                        
                        subTexDef.x = x;
                        subTexDef.y = y;
                        
                        glyphDefs[index].offsetX = (int32_t) (metrics[index].horiBearingX >> 6);
                        glyphDefs[index].offsetY = (int32_t) (maxBearing - (metrics[index].horiBearingY >> 6));
                        glyphDefs[index].advance = (int32_t) (metrics[index].horiAdvance >> 6);
                        
                        if (currBuffer) {
                            subTexDef.width = currBitmap->width;
                            subTexDef.height = currBitmap->rows;
                            
                            dest = &atlasBuffer[y * atlasSpan + x];
                            
                            for (int line=0;line<currBitmap->rows;line++) {
                                memcpy(&dest[line * atlasSpan], &currBuffer[line * currBitmap->pitch], currBitmap->width);
                            }
                        } else {
                            subTexDef.width = 0;
                            subTexDef.height = 0;
                        }
                        
                        // We want the key to use the proper extended ASCII value, so add InitialSupportedCharacterOffset
                        std::string key = fontKeyBase + std::to_string(index + InitialSupportedCharacterOffset);
                        
                        subTexDefs[key] = subTexDef;
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
                
                BGE::Game::getInstance()->getTextureService()->namedTextureAtlasFromBuffer("font", atlasBuffer, BGE::TextureFormat::Alpha, atlasW, atlasH, subTexDefs, [=](std::shared_ptr<BGE::TextureAtlas> atlas, std::shared_ptr<BGE::Error> error) -> void {
                    if (atlas) {
                        textureAtlas_ = atlas;
                        glyphs_.clear();
                        
                        // Our space is used whenever we don't have a match
                        uint16_t code;
                        std::string key = fontKeyBase + std::to_string(InitialSupportedCharacterOffset);
                        std::shared_ptr<BGE::Texture> subTex = atlas->getSubTexture(key);
                        std::shared_ptr<FontGlyph> space = std::make_shared<FontGlyph>(this, subTex, glyphDefs[0].offsetX, glyphDefs[0].offsetY, glyphDefs[0].advance);
                        std::shared_ptr<FontGlyph> glyph;
                        
                        glyphs_[InitialSupportedCharacterOffset] = space;
                        
                        // Space is already done
                        for (int i=0;i<NumSupportedCharacters + 1;i++) {
                            code = i + InitialSupportedCharacterOffset;
                            key = fontKeyBase + std::to_string(code);
                            subTex = atlas->getSubTexture(key);
                            
                            if (subTex) {
                                glyph = std::make_shared<FontGlyph>(this, subTex, glyphDefs[i].offsetX, glyphDefs[i].offsetY, glyphDefs[i].advance);
                                
                                if (glyph) {
                                    glyphs_[code] = glyph;
                                } else {
                                    // Replace with SPACE
                                    glyphs_[code] = space;
                                }
                            } else {
                                // Replace with SPACE
                                glyphs_[code] = space;
                            }
                        }
                    }
                    
                    for (int i=0;i<NumSupportedCharacters;i++) {
                        if (bitmaps[i].buffer) {
                            free(bitmaps[i].buffer);
                        }
                    }
                    
                    free(atlasBuffer);
                    
                    if (callback) {
                        valid_ = true;
                        callback(derived_shared_from_this<FontOpenGLES2>(), nullptr);
                    }
                });
            } else if (callback) {
                callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorAllocation));
            }
        } else if (callback) {
            callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType));
        }
    } else if (callback) {
        callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType));
    }
}

void BGE::FontOpenGLES2::drawString(std::string str, Vector2 &position, Vector4 &color, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    if (textureAtlas_ && textureAtlas_->isValid()) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        float x = position.x;
        float y = position.y;
        float gridX = x;
        float gridY = y;
        std::shared_ptr<BGE::RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<BGE::RenderServiceOpenGLES2>(BGE::Game::getInstance()->getRenderService());
        
        const char *chars = str.c_str();
        std::shared_ptr<FontGlyph> glyph;
        uint16_t code;
        size_t length = str.length();
        std::shared_ptr<BGE::TextureOpenGLES2> oglTex;
        
        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(renderer->pushShaderProgram("Font"));
        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
        GLint positionLocation = glShader->locationForAttribute("Position");
        
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(texCoordLocation);
        
        GLint textureUniform = glShader->locationForUniform("Texture");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        GLint colorUniform = glShader->locationForUniform("SourceColor");
        
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) renderer->getProjectionMatrix()->m);
        
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        
        glActiveTexture(GL_TEXTURE0);

        uint16_t prev = 0;
        
        for (int i=0;i<length;i++) {
            code = chars[i];
            
            glyph = glyphs_[code];
            
            if (glyph) {
                if (i == 0 && !minimum) {
                    x += glyph->getOffsetX();
                } else {
                    x += glyph->getOffsetX();
                }
                
                if (hasKerning() && prev) {
                    int32_t kerning = kerningForPair(prev, code);
                    x += kerning;
                    gridX += kerning;
                }

                y += glyph->getOffsetY();
                
                oglTex = std::dynamic_pointer_cast<BGE::TextureOpenGLES2>(glyph->getTexture());
                
                if (oglTex && oglTex->isValid()) {
                    const Vector2 *xys = oglTex->getXYs();
                    const Vector2 *uvs = oglTex->getUVs();
                    
                    vertices[0].position.x = x + xys[0].x;
                    vertices[0].position.y = y + xys[0].y;
                    vertices[0].position.z = 0;
                    vertices[0].tex.x = uvs[0].x;
                    vertices[0].tex.y = uvs[0].y;
                    
                    vertices[1].position.x = x + xys[1].x;
                    vertices[1].position.y = y + xys[1].y;
                    vertices[1].position.z = 0;
                    vertices[1].tex.x = uvs[1].x;
                    vertices[1].tex.y = uvs[1].y;
                    
                    vertices[2].position.x = x + xys[2].x;
                    vertices[2].position.y = y + xys[2].y;
                    vertices[2].position.z = 0;
                    vertices[2].tex.x = uvs[2].x;
                    vertices[2].tex.y = uvs[2].y;
                    
                    vertices[3].position.x = x + xys[3].x;
                    vertices[3].position.y = y + xys[3].y;
                    vertices[3].position.z = 0;
                    vertices[3].tex.x = uvs[3].x;
                    vertices[3].tex.y = uvs[3].y;
                    
                    glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
                    //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    
                    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), &vertices[0]);
                    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                    
                    glUniform1i(textureUniform, 0);
                    glUniform4f(colorUniform, 1, 0, 0, 1);
                    
                    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                                   GL_UNSIGNED_BYTE, &indices[0]);
                }
                
                gridX += glyph->getAdvance();
                x = gridX;
                y = gridY;
            }
            
            prev = code;
        }
    }
}
