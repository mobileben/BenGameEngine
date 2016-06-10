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
    std::string key = BGE::FontService::fontAsKey(name, pixelSize);
    
    auto fontIt = fonts_.find(key);
    
    if (fontIt != fonts_.end()) {
        return fonts_[key];
    } else {
        return nullptr;
    }
}

#define FULLFONT    1

void BGE::FontServiceOpenGLES2::loadFont(std::string name, uint32_t pixelSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback)
{
    
    std::string fontKey = BGE::FontService::fontAsKey(name, pixelSize);

    auto fontIt = fonts_.find(fontKey);
    
    if (fontIt != fonts_.end()) {
        std::shared_ptr<Font> font = fonts_[fontKey];
        
        if (callback) {
            callback(font, nullptr);
        }
    } else {
        auto fontResourcesIt = fontResources_.find(name);
        
        if (fontResourcesIt != fontResources_.end()) {
            
            std::string filename = fontResourcesIt->second;
            
            if (filename.length() > 0) {
                auto underlyingName = BGE::FontService::fontAsKey(filename, pixelSize);
                auto underlyingFontIt = underlyingFonts_.find(underlyingName);
                
                if (underlyingFontIt != underlyingFonts_.end()) {
                    if (underlyingFontIt->second) {
                        fonts_[fontKey] = underlyingFontIt->second;
                    }
                    
                    if (callback) {
                        callback(underlyingFontIt->second, nullptr);
                    }
                } else {
                    std::string fontName = filename;
                    
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
                        std::shared_ptr<BGE::FontOpenGLES2> oglFont = std::make_shared<BGE::FontOpenGLES2>(fontName, pixelSize);
                        
                        if (oglFont) {
                            oglFont->load(cpath, [this, fontKey, underlyingName, callback](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
                                if (font) {
                                    fonts_[fontKey] = font;
                                    underlyingFonts_[underlyingName] = font;
                                }
                                
                                if (callback) {
                                    callback(font, error);
                                }
                            });
                        } else {
                            if (callback) {
                                callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorOS));
                            }
                        }
                    } else {
                        // We have no resource associated with this font
                        if (callback) {
                            callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
                        }
                    }
                }
            } else {
                // We have no resource associated with this font
                if (callback) {
                    callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
                }
            }
        } else {
            // We have no resource associated with this font
            if (callback) {
                callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
            }
        }
    }
}


void BGE::FontServiceOpenGLES2::unloadFont(std::string name, uint32_t pixelSize) {
    
}


