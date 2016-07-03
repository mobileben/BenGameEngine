//
//  FontService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FontService.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <cassert>

NSBundle *BGE::FontService::builtinBundle_ = nil;
NSBundle *BGE::FontService::mainBundle_ = nil;

bool fontInfoIsEqual(std::shared_ptr<BGE::FontInfo> i, std::shared_ptr<BGE::FontInfo> j) {
    return i->family == j->family && i->style == j->style && i->asset == j->asset;
}

std::string BGE::FontService::fontAsKey(std::string name, uint32_t pixelSize) {
    return name + std::to_string(pixelSize);
}

void BGE::FontService::mapBundles(std::string bundleName)
{
    // TODO: Create FileService for things like this bundle crap
    
    if (!FontService::builtinBundle_) {
        FontService::builtinBundle_ = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:[[NSString alloc] initWithCString:bundleName.c_str() encoding:NSUTF8StringEncoding] withExtension:@"bundle"]];
        FontService::mainBundle_ = [NSBundle mainBundle];
    }
}

BGE::FontService::FontService(std::map<std::string, std::string> resources) : handleService_(InitialFontReserve, FontHandleService::NoMaxLimit) {
    FontService::mapBundles("BenGameEngineBundle");

    std::vector<std::string> assets;
    std::vector<std::string> names;
    
    fontResources_["default"] = "Arial Black.ttf";
    fontResources_["Avenir"] = "Avenir.ttc";
    fontResources_.insert(resources.begin(), resources.end());
    
    // Build FontInfo for all font resources
    for (auto f : fontResources_) {
        names.push_back(f.first);
        assets.push_back(f.second);
    }
    
    std::unique(assets.begin(), assets.end());
    
    // Assets will be unique
    for (auto f : assets) {
        buildFontInfoForAsset(f);
    }
    
    std::unique(fontInfo_.begin(), fontInfo_.end(), fontInfoIsEqual);
    
    // Build font table based on font names/aliases and
    for (auto fi : fontInfo_) {
        names.push_back(fi->name());
    }
    
    std::unique(names.begin(), names.end());
    
    for (auto name : names) {
        auto it = fontResources_.find(name);
        bool found = false;
        
        if (it != fontResources_.end()) {
            // Find the font info with our name it will either an exact match,
            
            for (auto fi : fontInfo_) {
                if (fi->name() == name) {
                    // This is the right one
                    fontTable_[name] = fi;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                // This is probably an alias, so find the non-style version of the asset
                for (auto fi : fontInfo_) {
                    if (fi->asset == it->second && fi->faceIndex == 0) {
                        // This is the right one
                        fontTable_[name] = fi;
                        found = true;
                        break;
                    }
                }
            }
        } else {
            for (auto fi : fontInfo_) {
                if (fi->name() == name) {
                    // This is the right one
                    fontTable_[name] = fi;
                    found = true;
                    break;
                }
            }
        }
        
        assert(found);
    }
}

// TODO: Move to file service
std::string BGE::FontService::pathForAsset(std::string asset) {
    // Find our fullpath
    NSString *str = [[NSString alloc] initWithCString:asset.c_str() encoding:NSUTF8StringEncoding];
    
    // First try BGE Bundle
    NSString *path = [FontService::builtinBundle_ pathForResource:str ofType:nil];
    
    // Next try main bundle
    if (!path) {
        path = [FontService::mainBundle_ pathForResource:str ofType:nil];
    }
    
    const char *cpath = NULL;
    
    if (path) {
        cpath = [path UTF8String];
        
        if (cpath) {
            return cpath;
        }
    }
    
    return "";
}

void BGE::FontService::buildFontInfoForAsset(std::string asset) {
    std::string path = pathForAsset(asset);
    
    if (path.length() > 0) {
        FT_Face face = NULL;
        FT_Error error = 0;
        FT_Library library;
        
        error = FT_Init_FreeType(&library);
     
        if (!error) {
            error = FT_New_Face(library, path.c_str(), 0, &face);

            if (!error) {
                FT_Long numFaces = face->num_faces;
                
                FT_Done_Face(face);
                
                for (auto fi=0;fi<numFaces;fi++) {
                    error = FT_New_Face(library, path.c_str(), fi, &face);
                    
                    if (!error) {
                        std::string family;
                        std::string style;
                        std::shared_ptr<FontInfo> fontInfo = std::make_shared<FontInfo>();

                        if (face->family_name) {
                            fontInfo->family = face->family_name;
                            
                            if (face->style_name) {
                                fontInfo->style = face->style_name;
                                
                                if (!strcmp(face->style_name, "Regular")) {
                                    // "Regular" style will also have an extra blank version
                                    std::shared_ptr<FontInfo> extra = std::make_shared<FontInfo>();
                                    
                                    extra->family = face->family_name;
                                    extra->asset = asset;
                                    
                                    // Only push if a family name exists
                                    fontInfo_.push_back(extra);
                                }
                            }
                            
                            fontInfo->asset = asset;
                            fontInfo->faceIndex = fi;

                            // Only push if a family name exists
                            fontInfo_.push_back(fontInfo);
                        }
                        
                        FT_Done_Face(face);
                    }
                }
            }
        }
    }
}

BGE::FontHandle BGE::FontService::getFontHandle(ObjectId fontId) {
    for (auto fontInfo : fontInfo_) {
        for (auto it : fontInfo->fonts) {
            auto font = getFont(it.second);
            
            if (font->getInstanceId() == fontId) {
                return it.second;
            }
        }
    }
    
    return FontHandle();
}

BGE::FontHandle BGE::FontService::getFontHandle(std::string name, uint32_t pixelSize) {
    auto f = fontTable_.find(name);
    
    if (f != fontTable_.end()) {
        auto info = f->second;
        auto font = info->fonts.find(pixelSize);
        
        if (font != info->fonts.end()) {
            return font->second;
        }
    }
    
    return FontHandle();
}

BGE::Font *BGE::FontService::getFont(ObjectId fontId) {
    for (auto fontInfo : fontInfo_) {
        for (auto it : fontInfo->fonts) {
            auto font = getFont(it.second);
            
            if (font->getInstanceId() == fontId) {
                return font;
            }
        }
    }
    
    return nullptr;
}

BGE::Font *BGE::FontService::getFont(std::string name, uint32_t pixelSize) {
    auto f = fontTable_.find(name);
    
    if (f != fontTable_.end()) {
        auto info = f->second;
        auto font = info->fonts.find(pixelSize);
        
        if (font != info->fonts.end()) {
            return getFont(font->second);
        }
    }
    
    return nullptr;
}

BGE::Font *BGE::FontService::getFont(FontHandle handle) {
    return handleService_.dereference(handle);
}

void BGE::FontService::removeFont(ObjectId fontId) {
    for (auto fontInfo : fontInfo_) {
        for (auto it : fontInfo->fonts) {
            auto font = getFont(it.second);
            
            if (font->getInstanceId() == fontId) {
                handleService_.release(it.second);
                fontInfo->fonts.erase(it.first);
                return;
            }
        }
    }
}

void BGE::FontService::removeFont(std::string name, uint32_t pixelSize) {
    auto f = fontTable_.find(name);
    
    if (f != fontTable_.end()) {
        auto info = f->second;
        auto font = info->fonts.find(pixelSize);
        
        if (font != info->fonts.end()) {
            handleService_.release(font->second);
            info->fonts.erase(font->first);
            return;
        }
    }
}

void BGE::FontService::removeFont(FontHandle handle) {
    for (auto fontInfo : fontInfo_) {
        for (auto it : fontInfo->fonts) {
            if (it.second == handle) {
                handleService_.release(it.second);
                fontInfo->fonts.erase(it.first);
                return;
            }
        }
    }
}

void BGE::FontService::loadFont(std::string name, uint32_t pxSize, std::function<void(FontHandle, std::shared_ptr<Error>)> callback) {
    auto entry = fontTable_.find(name);
    
    if (entry != fontTable_.end()) {
        auto info = entry->second;
        auto font = info->fonts.find(pxSize);
        
        if (font != info->fonts.end()) {
            // Our font exists
            if (callback) {
                callback(font->second, nullptr);
            }
        } else {
            // Create a new font
            std::string path = pathForAsset(info->asset);
            
            if (path.length() > 0) {
                FontHandle handle;
                Font *tFont = handleService_.allocate(handle);
                
                if (tFont) {
                    ObjectId fontId = getIdAndIncrement();
                    
                    tFont->initialize(handle, fontId, name, pxSize);
                    tFont->status_ = FontStatus::Loading;
                    
                    info->fonts[pxSize] = handle;
                    
                    tFont->load(path, info->faceIndex, [this, name, pxSize, callback](FontHandle fontHandle, std::shared_ptr<Error> error) -> void {
                        auto font = getFont(fontHandle);
                        
                        if (!font) {
                            auto entry = fontTable_.find(name);
                            
                            if (entry != fontTable_.end()) {
                                auto info = entry->second;
                                auto f = info->fonts.find(pxSize);
                                
                                if (f != info->fonts.end()) {
                                    auto tFont = getFont(f->second);
                                    
                                    if (tFont) {
                                        if (tFont->status_ == FontStatus::Loading) {
                                            info->fonts.erase(pxSize);
                                            handleService_.release(f->second);
                                        }
                                    }
                                }
                            }
                        }
                        
                        if (callback) {
                            callback(fontHandle, error);
                        }
                    });
                } else if (callback) {
                    callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorOS));
                }
                
            } else if (callback) {
                callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
            }
        }
    } else if (callback) {
        callback(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorNotInTable));
    }
}

void BGE::FontService::unloadFont(std::string name, uint32_t pixelSize) {
    
}

void BGE::FontService::unloadFont(FontHandle handle) {
}

