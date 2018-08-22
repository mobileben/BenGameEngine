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

#if TARGET_OS_IPHONE
NSBundle *BGE::FontService::builtinBundle_ = nil;
NSBundle *BGE::FontService::mainBundle_ = nil;
#endif /* TARGET_OS_IPHONE */

bool fontInfoIsEqual(std::shared_ptr<BGE::FontInfo> i, std::shared_ptr<BGE::FontInfo> j) {
    return i->family == j->family && i->style == j->style && i->asset == j->asset;
}

std::string BGE::FontService::fontAsKey(std::string name, uint32_t pixelSize) {
    return name + std::to_string(pixelSize);
}

#if TARGET_OS_IPHONE
void BGE::FontService::mapBundles(std::string bundleName)
{
    // TODO: Create FileService for things like this bundle crap
    if (!FontService::builtinBundle_) {
        FontService::builtinBundle_ = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:[[NSString alloc] initWithCString:bundleName.c_str() encoding:NSUTF8StringEncoding] withExtension:@"bundle"]];
        FontService::mainBundle_ = [NSBundle mainBundle];
    }
}
#endif /* TARGET_OS_IPHONE */

BGE::FontService::FontService(std::map<std::string, std::string> resources) : handleService_(InitialFontReserve, HandleServiceNoMaxLimit) {
#if TARGET_OS_IPHONE
    FontService::mapBundles("BenGameEngineBundle");

    std::vector<std::string> assets;
    std::vector<std::string> names;
    
    fontResources_["default"] = "Arial Black.ttf";
    fontResources_["Avenir"] = "Avenir.ttc";
    fontResources_["Avenir Next Condensed"] = "Avenir Next Condensed.ttc";
    fontResources_["Arial Narrow"] = "Arial Narrow.ttf";
    fontResources_["Arial Narrow Bold"] = "Arial Narrow Bold.ttf";
    fontResources_["Roboto Medium"] = "Roboto-Medium.ttf";
    fontResources_.insert(resources.begin(), resources.end());
    
    // Build FontInfo for all font resources
    for (auto const &f : fontResources_) {
        names.push_back(f.first);
        assets.push_back(f.second);
    }
    
    std::unique(assets.begin(), assets.end());
    
    // Assets will be unique
    for (auto const &f : assets) {
        buildFontInfoForAsset(f);
    }
    
    std::unique(fontInfo_.begin(), fontInfo_.end(), fontInfoIsEqual);
    
    // Build font table based on font names/aliases and
    for (auto const &fi : fontInfo_) {
        names.push_back(fi->name());
    }
    
    std::unique(names.begin(), names.end());
    
    for (auto const &name : names) {
        auto it = fontResources_.find(name);
        bool found = false;
        
        if (it != fontResources_.end()) {
            // Find the font info with our name it will either an exact match,
            
            for (auto const &fi : fontInfo_) {
                if (fi->name() == name) {
                    // This is the right one
                    fontTable_[name] = fi;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                // This is probably an alias, so find the non-style version of the asset
                for (auto const &fi : fontInfo_) {
                    if (fi->asset == it->second && fi->faceIndex == 0) {
                        // This is the right one
                        fontTable_[name] = fi;
                        found = true;
                        break;
                    }
                }
            }
        } else {
            for (auto const &fi : fontInfo_) {
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
#endif /* TARGET_OS_IPHONE */
}

uint32_t BGE::FontService::numFonts() {
    uint32_t num = 0;
    std::lock_guard<std::mutex> lock(fontInfoMutex_);

    for (auto const &fontInfo : fontInfo_) {
        for (auto const &it : fontInfo->fonts) {
            if (handleService_.dereference(it.second)) {
                num++;
            }
        }
    }
    
    return num;
}

uint32_t BGE::FontService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::FontService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::FontService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::FontService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

size_t BGE::FontService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::FontService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::FontService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

// TODO: Move to file service
std::string BGE::FontService::pathForAsset(std::string asset) {
#if TARGET_OS_IPHONE
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
#endif /* TARGET_OS_IPHONE */

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

                std::lock_guard<std::mutex> lock(fontInfoMutex_);

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
            
            FT_Done_FreeType(library);
        }
    }
}

BGE::FontHandle BGE::FontService::getFontHandle(std::string name, uint32_t pixelSize) {
    std::lock_guard<std::mutex> lock(fontTableMutex_);
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

BGE::Font *BGE::FontService::getFont(std::string name, uint32_t pixelSize) {
    std::lock_guard<std::mutex> lock(fontTableMutex_);
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

void BGE::FontService::removeFont(ScenePackageHandle scenePackageHandle, FontHandle handle) {
    std::lock_guard<std::recursive_mutex> lock(fontScenePackageMutex_);
    auto sceneIt = fontScenePackages_.find(handle);
    
    if (sceneIt != fontScenePackages_.end()) {
        for (auto scene=sceneIt->second.begin();scene != sceneIt->second.end();scene++) {
            if (*scene == scenePackageHandle) {
                // Need to erase prior to checking if the font has anymore references
                sceneIt->second.erase(scene);
                
                if (!fontHasReferences(handle)) {
                    removeFont(handle);
                }
                return;
            }
        }
    } else if (!fontHasReferences(handle)) {
        removeFont(handle);
    }
}

void BGE::FontService::removeFont(SpaceHandle spaceHandle, FontHandle handle) {
    std::lock_guard<std::recursive_mutex> lock(fontSpacesMutex_);
    auto spaceIt = fontSpaces_.find(handle);
    
    if (spaceIt != fontSpaces_.end()) {
        for (auto space=spaceIt->second.begin();space != spaceIt->second.end();space++) {
            if (*space == spaceHandle) {
                // Need to erase prior to checking if the font has anymore references
                spaceIt->second.erase(space);
                
                if (!fontHasReferences(handle)) {
                    removeFont(handle);
                }
                return;
            }
        }
    } else if (!fontHasReferences(handle)) {
        removeFont(handle);
    }
}

void BGE::FontService::removeFont(FontHandle handle) {
    std::lock_guard<std::mutex> lock(fontInfoMutex_);
    for (auto const &fontInfo : fontInfo_) {
        for (auto const &it : fontInfo->fonts) {
            if (it.second == handle) {
                auto font = getFont(it.second);
                
                if (font) {
                    font->destroy();
                }
                
                handleService_.release(it.second);
                fontInfo->fonts.erase(it.first);
                return;
            }
        }
    }
}

bool BGE::FontService::fontHasReferences(FontHandle fontHandle) {
    std::lock_guard<std::recursive_mutex> lock(fontScenePackageMutex_);
    auto sceneIt = fontScenePackages_.find(fontHandle);
    
    if (sceneIt != fontScenePackages_.end()) {
        if (sceneIt->second.size() > 0) {
            return true;
        }
    }

    std::lock_guard<std::recursive_mutex> sLock(fontSpacesMutex_);
    auto spaceIt = fontSpaces_.find(fontHandle);

    if (spaceIt != fontSpaces_.end()) {
        if (spaceIt->second.size() > 0) {
            return true;
        }
    }
    
    return false;
}

std::pair<BGE::FontHandle, std::shared_ptr<BGE::Error>> BGE::FontService::createFont(std::string name, uint32_t pxSize) {
    std::lock_guard<std::mutex> lock(fontTableMutex_);
    std::lock_guard<std::mutex> iLock(fontInfoMutex_);
    auto entry = fontTable_.find(name);
    
    if (entry != fontTable_.end()) {
        auto info = entry->second;
        auto font = info->fonts.find(pxSize);
        
        if (font != info->fonts.end()) {
            return std::make_pair(font->second, nullptr);
        } else {
            // Create a new font
            std::string path = pathForAsset(info->asset);
            
            if (path.length() > 0) {
                FontHandle handle;
                Font *tFont = handleService_.allocate(handle);
                
                if (tFont) {
                    tFont->initialize(handle, name, pxSize);
                    tFont->status_ = FontStatus::Loading;
                    
                    info->fonts[pxSize] = handle;

                    FontHandle fontHandle;
                    std::shared_ptr<Error> error;
                    std::tie(fontHandle, error) = tFont->load(path, info->faceIndex);
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
                                        tFont->destroy();
                                        handleService_.release(f->second);

                                        info->fonts.erase(pxSize);
                                    }
                                }
                            }
                        }
                    }

                    return std::make_pair(fontHandle, error);
                } else {
                    return std::make_pair(FontHandle(),std::make_shared<Error>(Font::ErrorDomain, FontErrorOS));
                }
                
            } else {
                return std::make_pair(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
            }
        }
        return std::make_pair(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorNotInTable));
    } else {
        return std::make_pair(FontHandle(), std::make_shared<Error>(Font::ErrorDomain, FontErrorNotInTable));
    }
}

std::pair<BGE::FontHandle, std::shared_ptr<BGE::Error>> BGE::FontService::createFont(std::string name, uint32_t pxSize, ScenePackageHandle scenePackageHandle) {
    FontHandle fontHandle;
    std::shared_ptr<Error> error;
    std::lock_guard<std::recursive_mutex> lock(fontScenePackageMutex_);
    std::tie(fontHandle, error) = createFont(name, pxSize);
    if (!fontHandle.isNull()) {
        auto &scenePackages = fontScenePackages_[fontHandle];
        bool found = false;

        for (auto const &package : scenePackages) {
            if (scenePackageHandle == package) {
                found = true;
                break;
            }
        }
        if (!found) {
            scenePackages.push_back(scenePackageHandle);
        }
    }
    return std::make_pair(fontHandle, error);
}

std::pair<BGE::FontHandle, std::shared_ptr<BGE::Error>> BGE::FontService::createFont(std::string name, uint32_t pxSize, SpaceHandle spaceHandle) {
    FontHandle fontHandle;
    std::shared_ptr<Error> error;
    std::lock_guard<std::recursive_mutex> lock(fontSpacesMutex_);
    std::tie(fontHandle, error) = createFont(name, pxSize);
    if (!fontHandle.isNull()) {
        auto &spaces = fontSpaces_[fontHandle];
        bool found = false;

        for (auto const &space : spaces) {
            if (spaceHandle == space) {
                found = true;
                break;
            }
        }
        if (!found) {
            spaces.push_back(spaceHandle);
        }
    }
    return std::make_pair(fontHandle, error);
}

