//
//  TextureService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureService.h"

BGE::TextureService::TextureService(EAGLContext *context) : Service(), textureHandleService_(InitialTextureReserve, HandleServiceNoMaxLimit), textureAtlasHandleService_(InitialTextureAtlasReserve, HandleServiceNoMaxLimit) {
    textureLoader_ = [[GLKTextureLoader alloc] initWithSharegroup:context.sharegroup];
}

BGE::TextureService::~TextureService() {
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(ScenePackageHandle scenePackageHandle, std::string name) {
    auto scenePackage = sceneTextures_.find(scenePackageHandle);
    
    if (scenePackage != sceneTextures_.end()) {
        auto sceneTex = scenePackage->second.find(name);
        
        if (sceneTex != scenePackage->second.end()) {
            return sceneTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureHandle();
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(SpaceHandle spaceHandle, std::string name) {
    auto space = spaceTextures_.find(spaceHandle);
    
    if (space != spaceTextures_.end()) {
        auto spaceTex = space->second.find(name);
        
        if (spaceTex != space->second.end()) {
            return spaceTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureHandle();
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(TextureAtlasHandle atlasHandle, std::string name) {
    auto atlas = atlasTextures_.find(atlasHandle);
    
    if (atlas != atlasTextures_.end()) {
        auto atlasTex = atlas->second.find(name);
        
        if (atlasTex != atlas->second.end()) {
            return atlasTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(ScenePackageHandle scenePackageHandle, std::string name) {
    auto scenePackage = sceneTextureAtlases_.find(scenePackageHandle);
    
    if (scenePackage != sceneTextureAtlases_.end()) {
        auto sceneTex = scenePackage->second.find(name);
        
        if (sceneTex != scenePackage->second.end()) {
            return sceneTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureAtlasHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(SpaceHandle spaceHandle, std::string name) {
    auto space = spaceTextureAtlases_.find(spaceHandle);
    
    if (space != spaceTextureAtlases_.end()) {
        auto spaceTex = space->second.find(name);
        
        if (spaceTex != space->second.end()) {
            return spaceTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureAtlasHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(FontHandle fontHandle, std::string name) {
    auto font = fontTextureAtlases_.find(fontHandle);
    
    if (font != fontTextureAtlases_.end()) {
        auto fontTex = font->second.find(name);
        
        if (fontTex != font->second.end()) {
            return fontTex->second;
        }
    } else {
        assert(false);
    }
    
    return TextureAtlasHandle();
}

BGE::Texture *BGE::TextureService::getTexture(ScenePackageHandle scenePackageHandle, std::string name) {
    return textureHandleService_.dereference(getTextureHandle(scenePackageHandle, name));
}

BGE::Texture *BGE::TextureService::getTexture(SpaceHandle spaceHandle, std::string name) {
    return textureHandleService_.dereference(getTextureHandle(spaceHandle, name));
}

BGE::Texture *BGE::TextureService::getTexture(TextureAtlasHandle atlasHandle, std::string name) {
    return textureHandleService_.dereference(getTextureHandle(atlasHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(ScenePackageHandle scenePackageHandle, std::string name) {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(scenePackageHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(SpaceHandle spaceHandle, std::string name) {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(spaceHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(FontHandle fontHandle, std::string name) {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(fontHandle, name));
}

void BGE::TextureService::removeTexture(ScenePackageHandle scenePackageHandle, TextureHandle handle) {
    auto scenePackage = sceneTextures_.find(scenePackageHandle);
    
    if (scenePackage != sceneTextures_.end()) {
        for (auto sceneTex : scenePackage->second) {
            if (sceneTex.second == handle) {
                textureHandleService_.release(handle);
                scenePackage->second.erase(sceneTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTexture(SpaceHandle spaceHandle, TextureHandle handle) {
    auto space = spaceTextures_.find(spaceHandle);
    
    if (space != spaceTextures_.end()) {
        for (auto spaceTex : space->second) {
            if (spaceTex.second == handle) {
                textureHandleService_.release(handle);
                space->second.erase(spaceTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(ScenePackageHandle scenePackageHandle, TextureAtlasHandle handle) {
    auto scenePackage = sceneTextureAtlases_.find(scenePackageHandle);
    
    if (scenePackage != sceneTextureAtlases_.end()) {
        for (auto sceneTex : scenePackage->second) {
            if (sceneTex.second == handle) {
                textureAtlasHandleService_.release(handle);
                scenePackage->second.erase(sceneTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(SpaceHandle spaceHandle, TextureAtlasHandle handle) {
    auto space = spaceTextureAtlases_.find(spaceHandle);
    
    if (space != spaceTextureAtlases_.end()) {
        for (auto spaceTex : space->second) {
            if (spaceTex.second == handle) {
                textureAtlasHandleService_.release(handle);
                space->second.erase(spaceTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(FontHandle fontHandle, TextureAtlasHandle handle) {
    auto font = fontTextureAtlases_.find(fontHandle);
    
    if (font != fontTextureAtlases_.end()) {
        for (auto fontTex : font->second) {
            if (fontTex.second == handle) {
                textureAtlasHandleService_.release(handle);
                font->second.erase(fontTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::createTextureFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &scenePackage = sceneTextures_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        createTextureFromFile(name, filename,  [this, name, scenePackageHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &scenePackage = sceneTextures_[scenePackageHandle];
                scenePackage[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &scenePackage = sceneTextures_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        createTextureFromBuffer(name, buffer, format, width, height, [this, name, scenePackageHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &scenePackage = sceneTextures_[scenePackageHandle];
                scenePackage[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &space = spaceTextures_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        createTextureFromFile(name, filename,  [this, name, spaceHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &space = spaceTextures_[spaceHandle];
                space[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &space = spaceTextures_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        createTextureFromBuffer(name, buffer, format, width, height, [this, name, spaceHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &space = spaceTextures_[spaceHandle];
                space[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureFromFile(TextureAtlasHandle atlasHandle, std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &atlas = atlasTextures_[atlasHandle];
    auto tex = atlas.find(name);
    
    if (tex == atlas.end()) {
        createTextureFromFile(name, filename,  [this, name, atlasHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &atlas = atlasTextures_[atlasHandle];
                atlas[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureFromBuffer(TextureAtlasHandle atlasHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback) {
    auto &atlas = atlasTextures_[atlasHandle];
    auto tex = atlas.find(name);
    
    if (tex == atlas.end()) {
        createTextureFromBuffer(name, buffer, format, width, height, [this, name, atlasHandle, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
            if (texture) {
                auto &atlas = atlasTextures_[atlasHandle];
                atlas[name] = texture->getHandle();
            }
            
            if (callback) {
                callback(texture, error);
            }
        });
    } else if (callback) {
        callback(textureHandleService_.dereference(tex->second), nullptr);
    }}

void BGE::TextureService::createTextureFromFile(std::string name, std::string filename, std::function<void(Texture *, std::shared_ptr<Error>)> callback)
{
    [textureLoader_ textureWithContentsOfFile:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding] options:@{ GLKTextureLoaderApplyPremultiplication: @(YES) } queue:nil completionHandler:^(GLKTextureInfo *textureInfo, NSError *error) {
        std::shared_ptr<Error> bgeError;
        
        if (textureInfo) {
            TextureHandle textureHandle;
            Texture *texture;
            
            texture = textureHandleService_.allocate(textureHandle);
            
            if (texture) {
                uint32_t texId = getIdAndIncrement();

                texture->initialize(textureHandle, texId, name, textureInfo);
            } else {
                std::string domain = [error.domain UTF8String];
                int32_t code = (uint32_t) error.code;
                std::shared_ptr<Error> underlying = std::make_shared<Error>(domain, code);
                bgeError = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorOS, underlying);
                
                // We had a problem, release the handle
                textureHandleService_.release(textureHandle);
            }
            
            if (callback) {
                callback(texture, bgeError);
            }
        } else {
            std::string domain = [error.domain UTF8String];
            int32_t code = (uint32_t) error.code;
            std::shared_ptr<Error> underlying = std::make_shared<BGE::Error>(domain, code);
            bgeError = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorOS, underlying);
            
            if (callback) {
                callback(nullptr, bgeError);
            }
        }
    }];
}

void BGE::TextureService::createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback)
{
    TextureHandle textureHandle;
    Texture *texture;

    texture = textureHandleService_.allocate(textureHandle);

    if (texture) {
        uint32_t texId = getIdAndIncrement();

        texture->initialize(textureHandle, texId, name);

        texture->createFromBuffer(buffer, format, width, height, [this, textureHandle, callback](Texture *newTexture, std::shared_ptr<Error> error) -> void {
            if (!newTexture) {
                // We had a problem, release the handle
                textureHandleService_.release(textureHandle);
            }
            
            if (callback) {
                callback(newTexture, error);
            }
        });
    } else {
        auto bgeError = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorClassAllocation);
        
        if (callback) {
            callback(nullptr, bgeError);
        }
    }
}

void BGE::TextureService::createTextureAtlasFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &scenePackage = sceneTextureAtlases_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        createTextureAtlasFromFile(name, filename, subTextureDefs, [this, name, scenePackageHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &scenePackage = sceneTextureAtlases_[scenePackageHandle];
                scenePackage[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &scenePackage = sceneTextureAtlases_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs, [this, name, scenePackageHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &scenePackage = sceneTextureAtlases_[scenePackageHandle];
                scenePackage[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &space = spaceTextureAtlases_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        createTextureAtlasFromFile(name, filename, subTextureDefs, [this, name, spaceHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &space = spaceTextureAtlases_[spaceHandle];
                space[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &space = spaceTextureAtlases_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs, [this, name, spaceHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &space = spaceTextureAtlases_[spaceHandle];
                space[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromFile(FontHandle fontHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &font = fontTextureAtlases_[fontHandle];
    auto tex = font.find(name);
    
    if (tex == font.end()) {
        createTextureAtlasFromFile(name, filename, subTextureDefs, [this, name, fontHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &font = fontTextureAtlases_[fontHandle];
                font[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromBuffer(FontHandle fontHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    auto &font = fontTextureAtlases_[fontHandle];
    auto tex = font.find(name);
    
    if (tex == font.end()) {
        createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs, [this, name, fontHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (atlas) {
                auto &font = fontTextureAtlases_[fontHandle];
                font[name] = atlas->getHandle();
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else if (callback) {
        callback(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

void BGE::TextureService::createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> bgeError;
    TextureAtlasHandle atlasHandle;
    TextureAtlas *atlas;
    
    atlas = textureAtlasHandleService_.allocate(atlasHandle);
    
    if (atlas) {
        uint32_t texId = getIdAndIncrement();

        atlas->initialize(atlasHandle, texId, name);
        
        atlas->createFromFile(filename, subTextureDefs, [this, atlasHandle, callback](TextureAtlas * atlas, std::shared_ptr<Error> error) -> void {
            if (!atlas) {
                // We had a problem, release the handle
                textureAtlasHandleService_.release(atlasHandle);
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else {
        bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorClassAllocation);
        
        if (callback) {
            callback(atlas, bgeError);
        }
    }
}

void BGE::TextureService::createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback) {
    std::shared_ptr<Error> bgeError;
    TextureAtlasHandle atlasHandle;
    TextureAtlas *atlas;
    
    atlas = textureAtlasHandleService_.allocate(atlasHandle);
    
    if (atlas) {
        uint32_t texId = getIdAndIncrement();

        atlas->initialize(atlasHandle, texId, name);
        
        atlas->createFromBuffer(buffer, format, width, height, subTextureDefs, [this, atlasHandle, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
            if (!atlas) {
                // We had a problem, release the handle
                textureAtlasHandleService_.release(atlasHandle);
            }
            
            if (callback) {
                callback(atlas, error);
            }
        });
    } else {
        bgeError = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorClassAllocation);
        
        if (callback) {
            callback(atlas, bgeError);
        }
    }
}

BGE::Texture *BGE::TextureService::createSubTexture(TextureAtlasHandle atlasHandle, std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated) {
    auto &atlasSubTex = atlasTextures_[atlasHandle];
    auto tex = atlasSubTex.find(name);
    
    if (tex == atlasSubTex.end()) {
        std::shared_ptr<Error> error;
        auto texture = createSubTexture(name, atlas, x, y, width, height, rotated);
        
        if (texture) {
            atlasSubTex[name] = texture->getHandle();
        }
        
        return texture;
    } else {
        return textureHandleService_.dereference(tex->second);
    }
}


BGE::Texture *BGE::TextureService::createSubTexture(std::string name, TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated) {
    TextureHandle textureHandle;
    Texture *texture;
    
    texture = textureHandleService_.allocate(textureHandle);
    
    if (texture) {
        uint32_t texId = getIdAndIncrement();

        texture->initialize(textureHandle, texId, name);

        auto error = texture->createSubTexture(atlas, x, y, width, height, rotated);
        
        if (error) {
            textureHandleService_.release(textureHandle);
            texture = nullptr;
        }
    }
    
    return texture;
}

