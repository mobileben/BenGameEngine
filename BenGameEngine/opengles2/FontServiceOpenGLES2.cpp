//
//  FontServiceOpenGLES2.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FontServiceOpenGLES2.h"
#include "FontOpenGLES2.h"
#include "TextureAtlas.h"
#include "Game.h"

static const float MetricScale = 1.0 / 64.0;
static const int InitialSupportedCharacterOffset = 32;
static const int NumSupportedCharacters = 256 - InitialSupportedCharacterOffset;   // Support extended ASCCI, ignore control characters (0-31)

NSBundle *BGE::FontServiceOpenGLES2::builtinBundle_ = nil;
NSBundle *BGE::FontServiceOpenGLES2::mainBundle_ = nil;

void BGE::FontServiceOpenGLES2::mapBundles(std::string bundleName)
{
    if (!FontServiceOpenGLES2::builtinBundle_) {
        FontServiceOpenGLES2::builtinBundle_ = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:[[NSString alloc] initWithCString:bundleName.c_str() encoding:NSUTF8StringEncoding] withExtension:@"bundle"]];
        FontServiceOpenGLES2::mainBundle_ = [NSBundle mainBundle];
    }
}

BGE::FontServiceOpenGLES2::FontServiceOpenGLES2(std::map<std::string, std::string> resources) : FontService(resources) {
}


std::shared_ptr<BGE::Font> BGE::FontServiceOpenGLES2::getFont(std::string name, uint32_t pixelSize) {
    std::string key = name + std::to_string(pixelSize);
    
    return fonts_[key];
}

#define FULLFONT    1

void BGE::FontServiceOpenGLES2::loadFont(std::string name, uint32_t pixelSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback)
{
    std::string fontKey = BGE::FontService::fontAsKey(name, pixelSize);
    std::shared_ptr<Font> font = fonts_[fontKey];
    
    if (font) {
        if (callback) {
            callback(font, nullptr);
        }
    } else {
        std::string filename = fontResources_[name];
        
        if (filename.length() > 0) {
            FT_Face face = NULL;
            FT_Error error = 0;
            
            // Find our fullpath
            NSString *str = [[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding];
            
            // First try BGE Bundle
            NSString *path = [builtinBundle_ pathForResource:str ofType:nil];
            
            // Next try main bundle
            if (!path) {
                path = [mainBundle_ pathForResource:str ofType:nil];
            }
            
            // Last try and convert system font to TTF
            const char *cpath = NULL;
            
            if (path) {
                cpath = [path UTF8String];
            }
            
            if (cpath) {
                std::shared_ptr<BGE::FontOpenGLES2> oglFont = std::make_shared<BGE::FontOpenGLES2>(name, pixelSize, cpath);
                
                if (oglFont) {
                    fonts_[fontKey] = oglFont;
                }
#if 0
                error = FT_New_Face(freetypeLibrary_, cpath, 0, &face);
                
                if (!error) {
                    error = FT_Set_Pixel_Sizes(face, 0, pixelSize);
                    
                    if (!error) {
                        FT_Glyph_Metrics metrics[NumSupportedCharacters];
                        FT_Bitmap bitmaps[NumSupportedCharacters];
                        int maxBearing = 0;
                        int minHang = 0;
                        int top;
                        int bottom;
                        int width;
                        int height;
                        int maxWidth = 0;
                        
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
                                } else {
                                    NSLog(@"Character %d doesn't have a map", i + 32);
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
                                
                                NSLog(@"%c (%d) width %ld, heigh %ld", i + 32, i + 32, metrics[i].width >> 6, metrics[i].height >> 6);
                                NSLog(@"Bitmap width %d, height %d", face->glyph->bitmap.width, face->glyph->bitmap.rows);
                            } else {
                                break;
                            }
                        }
                        
                        int glyphW = maxWidth;
                        int glyphH = maxBearing - minHang;
                        int glyphSize = glyphW * glyphH;
                        int maxCol = 16;
                        int maxRow = NumSupportedCharacters / maxCol;
                        int atlasW = glyphW * maxCol;
                        int atlasH = glyphH * maxRow;
                        int atlasSpan = glyphW * maxCol;
                        
#if FULLFONT
#if 0
                        if (!(atlasW && !(atlasW & (atlasW - 1)))) {
                            atlasW--;
                            atlasW |= atlasW >> 1;
                            atlasW |= atlasW >> 2;
                            atlasW |= atlasW >> 4;
                            atlasW |= atlasW >> 8;
                            atlasW |= atlasW >> 16;
                            atlasW++;
                        }
                        if (!(atlasH && !(atlasH & (atlasH - 1)))) {
                            atlasH--;
                            atlasH |= atlasH >> 1;
                            atlasH |= atlasH >> 2;
                            atlasH |= atlasH >> 4;
                            atlasH |= atlasH >> 8;
                            atlasH |= atlasH >> 16;
                            atlasH++;
                        }
#endif
#endif
                        
                        atlasSpan = atlasW;
                        
                        unsigned char *atlasBuffer = (unsigned char *)malloc(atlasW * atlasH);
                        
                        NSLog(@"maxBearing = %d, minHang = %d, width = %d", maxBearing, minHang, maxWidth);
                        
                        // Generate one larger texture for the atlas
                        int altWidth, altHeight;
                        if (atlasBuffer) {
                            int index;
                            int x = 0, y = 0;
                            unsigned char *dest;
                            unsigned char *currBuffer;
                            BGESubTextureDef subTexs[NumSupportedCharacters];
                            
                            memset(atlasBuffer, 0, atlasW * atlasH);
                            
                            // 16 columns, 14 rows
                            for (int col=0;col<maxCol;col++) {
                                x = col * glyphW;
                                
                                for (int row=0;row<maxRow;row++) {
                                    index = row * maxCol + col;
                                    
                                    y = row * glyphH;
                                    
                                    FT_Bitmap *currBitmap = &bitmaps[index];
                                    
                                    int yOffset = maxBearing - (metrics[index].horiBearingY >> 6);
                                    
                                    currBuffer = currBitmap->buffer;
                                    
                                    if (currBuffer) {
                                        dest = &atlasBuffer[(y + yOffset) * atlasSpan + x];
                                        
                                        for (int line=0;line<currBitmap->rows;line++) {
#if FULLFONT
                                            memcpy(&atlasBuffer[(y + line) * atlasSpan + x], &currBuffer[line * currBitmap->pitch], currBitmap->width);
#endif
                                        }
                                    }
                                    
#if !FULLFONT
                                    if (row == 2 && col == 0) {
                                        atlasW = currBitmap->width;
                                        atlasH = currBitmap->rows;
                                        
#if 0
                                        if (!(atlasW && !(atlasW & (atlasW - 1)))) {
                                            atlasW--;
                                            atlasW |= atlasW >> 1;
                                            atlasW |= atlasW >> 2;
                                            atlasW |= atlasW >> 4;
                                            atlasW |= atlasW >> 8;
                                            atlasW |= atlasW >> 16;
                                            atlasW++;
                                        }
                                        if (!(atlasH && !(atlasH & (atlasH - 1)))) {
                                            atlasH--;
                                            atlasH |= atlasH >> 1;
                                            atlasH |= atlasH >> 2;
                                            atlasH |= atlasH >> 4;
                                            atlasH |= atlasH >> 8;
                                            atlasH |= atlasH >> 16;
                                            atlasH++;
                                        }
#endif
                                        memcpy(atlasBuffer, currBitmap->buffer, currBitmap->pitch * currBitmap->rows);
                                    }
#endif
                                }
                            }
                            
                            Game::getInstance()->getTextureService()->namedTextureFromBuffer("font", TextureFormat::Alpha, atlasW, atlasH, atlasBuffer, glyphSize, nullptr);
                            
                            for (int i=0;i<NumSupportedCharacters;i++) {
                                if (bitmaps[i].buffer) {
                                    free(bitmaps[i].buffer);
                                }
                            }
                            free(atlasBuffer);
                        }
                    }
                }
#endif
            }
        }
    }
}


void BGE::FontServiceOpenGLES2::unloadFont(std::string name, uint32_t pixelSize) {
    
}


