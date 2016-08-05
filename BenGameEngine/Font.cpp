//
//  Font.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Font.h"
#include "Game.h"
#include "TransformComponent.h"
#include "TextureOpenGLES2.h"
#include "TextureAtlasOpenGLES2.h"
#include "RenderServiceOpenGLES2.h"
#include "ShaderServiceOpenGLES2.h"
#include "ft2build.h"

#include FT_FREETYPE_H

// InitialSupportedCharacterOffset is also ASCII for the space character
static const int InitialSupportedCharacterOffset = 32;
static const int NumSupportedCharacters = 256 - InitialSupportedCharacterOffset;   // Support extended ASCCI, ignore control characters (0-31)

const std::string BGE::Font::ErrorDomain = "Font";

BGE::Font::Font(ObjectId fontId) : NamedObject(fontId), handle_(FontHandle()), pixelSize_(0), status_(FontStatus::Invalid), valid_(false), textureAtlas_(nullptr), hasKerning_(false) {
}

BGE::Font::Font(std::string name, uint32_t pixelSize) : NamedObject(0, name), handle_(FontHandle()), pixelSize_(pixelSize), status_(FontStatus::Invalid), valid_(false), textureAtlas_(nullptr), hasKerning_(false) {
}

BGE::Font::~Font() {
    Game::getInstance()->getTextureService()->removeTexture(textureAtlas_);
}

void BGE::Font::initialize(FontHandle handle, ObjectId fontId, std::string name, uint32_t pixelSize) {
    setName(name);
    setInstanceId(fontId);
    handle_ = handle;
    pixelSize_ = pixelSize;
    status_ = FontStatus::Invalid;
    valid_ = false;
    
    if (textureAtlas_) {
        // TODO: this needs to support removing by reference
        Game::getInstance()->getTextureService()->removeTexture(textureAtlas_);
    }
    
    textureAtlas_ = nullptr;
    hasKerning_ = false;
}

std::shared_ptr<BGE::FontGlyph> BGE::Font::glyphForExtendedASCII(uint16_t code) {
    return glyphs_[code];
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

uint32_t BGE::Font::getStringWidth(std::string str, bool minimum) {
    int32_t width = 0;
    
    if (status_ == FontStatus::Valid) {
        const char *chars = str.c_str();
        std::shared_ptr<FontGlyph> glyph;
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

void BGE::Font::load(std::string filename, uint32_t faceIndex, std::function<void(FontHandle, std::shared_ptr<Error>)> callback)  {
    FT_Face face = NULL;
    FT_Error error = 0;
    FT_Library library;
    error = FT_Init_FreeType(&library);
    
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
                    
                    Game::getInstance()->getTextureService()->namedTextureAtlasFromBuffer(FontService::fontAsKey(getName(), pixelSize_), atlasBuffer, TextureFormat::Alpha, atlasW, atlasH, subTexDefs, [=](std::shared_ptr<TextureAtlas> atlas, std::shared_ptr<Error> error) -> void {
                        if (atlas) {
                            textureAtlas_ = atlas;
                            glyphs_.clear();
                            
                            // Our space is used whenever we don't have a match
                            uint16_t code;
                            std::string key = fontKeyBase + std::to_string(InitialSupportedCharacterOffset);
                            std::shared_ptr<Texture> subTex = atlas->getSubTexture(key);
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
                            status_ = FontStatus::Valid;
                            callback(handle_, nullptr);
                        }
                    });
                } else if (callback) {
                    callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorAllocation));
                }
            } else if (callback) {
                callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType));
            }
            
            FT_Done_Face(face);
        } else if (callback) {
            callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType));
        }
        
        FT_Done_FreeType(library);
    } else if (callback) {
        callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorFreeType));
    }
}

void BGE::Font::drawString(std::string str, Vector2 &position, Color &color, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    Matrix4 xform;
    Matrix4MakeTranslation(xform, position.x, position.y, 0);
    
    drawString(str, xform.m, color, horizAlignment, vertAlignment, minimum);
}


// TODO: Move to renderer
void BGE::Font::drawString(std::string str, std::shared_ptr<TransformComponent> transform, Color &color, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    
    drawString(str, transform->getMatrixRaw(), color, horizAlignment, vertAlignment, minimum);
}

// TODO: Move to renderer
void BGE::Font::drawString(std::string str, const float *rawMatrix, Color &color, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    if (str.length() > 0 && textureAtlas_ && textureAtlas_->isValid()) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        // TODO: Adjustment based on alignment to be done here
        float x = 0;
        float y = 0;
        std::shared_ptr<RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<RenderServiceOpenGLES2>(Game::getInstance()->getRenderService());
        
        const char *chars = str.c_str();
        std::shared_ptr<FontGlyph> glyph;
        uint16_t code;
        size_t length = str.length();
        std::shared_ptr<TextureOpenGLES2> oglTex;
        
        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(renderer->pushShaderProgram("Font"));
        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
        GLint positionLocation = glShader->locationForAttribute("Position");
        
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(texCoordLocation);
        
        GLint textureUniform = glShader->locationForUniform("Texture");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        GLint modelLocation = glShader->locationForUniform("ModelView");
        GLint colorUniform = glShader->locationForUniform("SourceColor");
        
        oglTex = std::dynamic_pointer_cast<TextureOpenGLES2>(textureAtlas_->getTexture());
        
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) renderer->getProjectionMatrix()->m);
        glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) rawMatrix);
        
        // Compute the offsets if needed
        switch (horizAlignment) {
            case FontHorizontalAlignment::Center:
                x -= getStringWidth(str, minimum) / 2.0;
                break;
                
            case FontHorizontalAlignment::Right:
                x -= getStringWidth(str, minimum);
                break;
                
            default:
                break;
        }
        
        float deltaY = 0;
        
        switch (vertAlignment) {
            case FontVerticalAlignment::Center:
                deltaY = getHeight() / 2.0;
                break;
                
            case FontVerticalAlignment::Bottom:
                deltaY = getHeight();
                break;
                
            case FontVerticalAlignment::Baseline:
                deltaY = getBaseline();
                break;
                
            default:
                break;
        }
        
        y -= deltaY;
        
        float gridX = x;
        float gridY = y;
        
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        
        glActiveTexture(GL_TEXTURE0);
        
        uint16_t prev = 0;
        
        glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glUniform1i(textureUniform, 0);
        glUniform4f(colorUniform, color.r, color.g, color.b, color.a);
        
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
                
                oglTex = std::dynamic_pointer_cast<TextureOpenGLES2>(glyph->getTexture());
                
                if (oglTex) {
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
                    
                    // TODO texture stack here
                    //                    glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
                    
                    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), &vertices[0]);
                    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                    
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
