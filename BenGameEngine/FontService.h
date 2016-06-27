//
//  FontService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontService_h
#define BGEFontService_h

#include <stdio.h>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "Service.h"
#include "FontInfo.h"
#include "Font.h"
#include "Error.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <Foundation/Foundation.h>

namespace BGE {
    class FontService : public BGE::Service
    {
    public:
        static std::string fontAsKey(std::string name, uint32_t pixelSize);
        static void mapBundles(std::string bundleName);
        
        FontService(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        virtual ~FontService() {}
        
        FT_Library getFreetypeLibrary() const { return freetypeLibrary_; }
        std::shared_ptr<Font> getFont(std::string name, uint32_t pixelSize);
        
        template <typename T, typename... Args> std::shared_ptr<T> createFont(Args&& ...args) {
            static_assert(std::is_base_of<Font, T>::value, "Not Font");
            std::shared_ptr<T> font = T::create(std::forward<Args>(args)...);
            
            return font;
        }

        template <typename T>
        void loadFont(std::string name, uint32_t pxSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback) {
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
                        std::shared_ptr<T> tFont = this->createFont<T>(name, pxSize);
                        
                        if (tFont) {
                            tFont->status_ = FontStatus::Loading;
                            
                            info->fonts[pxSize] = tFont;
                            
                            tFont->load(path, info->faceIndex, [this, name, pxSize, callback](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
                                if (font) {
                                    font->status_ = FontStatus::Valid;
                                } else {
                                    auto entry = fontTable_.find(name);
                                    
                                    if (entry != fontTable_.end()) {
                                        auto info = entry->second;
                                        auto f = info->fonts.find(pxSize);
                                        
                                        if (f != info->fonts.end()) {
                                            if (f->second->status_ == FontStatus::Loading) {
                                                info->fonts.erase(pxSize);
                                            }
                                        }
                                    }
                                }
                                
                                if (callback) {
                                    callback(font, error);
                                }
                            });
                        } else if (callback) {
                            callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorOS));
                        }
                        
                    } else if (callback) {
                        callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorNoResourceFile));
                    }
                }
            } else if (callback) {
                callback(nullptr, std::make_shared<Error>(Font::ErrorDomain, FontErrorNotInTable));
            }
        }
        
        virtual void unloadFont(std::string name, uint32_t pixelSize) =0;
        
    protected:
        // TODO: For now this is Mac/iOS specific
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
        
        FT_Library freetypeLibrary_;
        
        std::map<std::string, std::string> fontResources_;
        std::map<std::string, std::shared_ptr<FontInfo>> fontTable_;
        std::vector<std::shared_ptr<FontInfo>> fontInfo_;
        
        std::string pathForAsset(std::string asset);
        
    private:
        void buildFontInfoForAsset(std::string asset);
    };
}

#endif /* BGEFontService_h */
