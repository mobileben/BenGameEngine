//
//  TextureService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureService.h"
#include "Game.h"
#include "RawTexture.h"

BGE::TextureService::TextureService(EAGLContext *context) : Service(), textureHandleService_(InitialTextureReserve, HandleServiceNoMaxLimit), textureAtlasHandleService_(InitialTextureAtlasReserve, HandleServiceNoMaxLimit) {
    textureLoader_ = [[GLKTextureLoader alloc] initWithSharegroup:context.sharegroup];
}

void BGE::TextureService::garbageCollect() {
    textureHandleService_.garbageCollect();
    textureAtlasHandleService_.garbageCollect();
}

uint32_t BGE::TextureService::numTextures() const {
    std::vector<TextureHandle> textures;
    auto packageService = Game::getInstance()->getScenePackageService();
    auto spaceService = Game::getInstance()->getSpaceService();
    
    for (auto const &packageIt : packageTextures_) {
        auto package = packageService->getScenePackage(packageIt.first);
        
        if (package) {
            for (auto &texIt : packageIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &spaceIt : spaceTextures_) {
        auto space = spaceService->getSpace(spaceIt.first);
        
        if (space) {
            for (auto &texIt : spaceIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &atlasIt : atlasTextures_) {
        auto atlas = getTextureAtlas(atlasIt.first);
        
        if (atlas) {
            for (auto &texIt : atlasIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    return (uint32_t) textures.size();
}

uint32_t BGE::TextureService::numSubTextures() const {
    std::vector<TextureHandle> textures;
    auto packageService = Game::getInstance()->getScenePackageService();
    auto spaceService = Game::getInstance()->getSpaceService();
    
    for (auto const &packageIt : packageTextures_) {
        auto package = packageService->getScenePackage(packageIt.first);
        
        if (package) {
            for (auto &texIt : packageIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex && tex->isSubTexture()) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &spaceIt : spaceTextures_) {
        auto space = spaceService->getSpace(spaceIt.first);
        
        if (space) {
            for (auto &texIt : spaceIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex && tex->isSubTexture()) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &atlasIt : atlasTextures_) {
        auto atlas = getTextureAtlas(atlasIt.first);
        
        if (atlas) {
            for (auto &texIt : atlasIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex && tex->isSubTexture()) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    return (uint32_t) textures.size();
}

uint32_t BGE::TextureService::numTextureAtlases() const {
    std::vector<TextureAtlasHandle> atlases;
    auto packageService = Game::getInstance()->getScenePackageService();
    auto spaceService = Game::getInstance()->getSpaceService();
    auto fontService = Game::getInstance()->getFontService();
    
    for (auto const &packageIt : packageTextureAtlases_) {
        auto package = packageService->getScenePackage(packageIt.first);
        
        if (package) {
            for (auto &atlasIt : packageIt.second) {
                auto atlas = getTextureAtlas(atlasIt.second);
                
                if (atlas) {
                    if (std::find(atlases.begin(), atlases.end(), atlasIt.second) == atlases.end()) {
                        atlases.push_back(atlasIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &spaceIt : spaceTextureAtlases_) {
        auto space = spaceService->getSpace(spaceIt.first);
        
        if (space) {
            for (auto &atlasIt : spaceIt.second) {
                auto atlas = getTextureAtlas(atlasIt.second);
                
                if (atlas) {
                    if (std::find(atlases.begin(), atlases.end(), atlasIt.second) == atlases.end()) {
                        atlases.push_back(atlasIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &fontIt : fontTextureAtlases_) {
        auto font = fontService->getFont(fontIt.first);
        
        if (font) {
            for (auto &atlasIt : fontIt.second) {
                auto atlas = getTextureAtlas(atlasIt.second);
                
                if (atlas) {
                    if (std::find(atlases.begin(), atlases.end(), atlasIt.second) == atlases.end()) {
                        atlases.push_back(atlasIt.second);
                    }
                }
            }
        }
    }
    
    return (uint32_t) atlases.size();
}

uint32_t BGE::TextureService::numUsedTextureHandles() const {
    return textureHandleService_.numUsedHandles();
}

uint32_t BGE::TextureService::maxTextureHandles() const {
    return textureHandleService_.capacity();
}

uint32_t BGE::TextureService::numTextureHandleResizes() const {
    return textureHandleService_.numResizes();
}

uint32_t BGE::TextureService::maxTextureHandlesAllocated() const {
    return textureHandleService_.getMaxAllocated();
}

uint32_t BGE::TextureService::numUsedTextureAtlasHandles() const {
    return textureAtlasHandleService_.numUsedHandles();
}

uint32_t BGE::TextureService::maxTextureAtlasHandles() const {
    return textureAtlasHandleService_.capacity();
}

uint32_t BGE::TextureService::numTextureAtlasHandleResizes() const {
    return textureAtlasHandleService_.numResizes();
}

uint32_t BGE::TextureService::maxTextureAtlasHandlesAllocated() const {
    return textureAtlasHandleService_.getMaxAllocated();
}

size_t BGE::TextureService::usedHandleMemory() const {
    return textureHandleService_.usedMemory() + textureAtlasHandleService_.usedMemory();
}

size_t BGE::TextureService::unusedHandleMemory() const {
    return textureHandleService_.unusedMemory() + textureAtlasHandleService_.unusedMemory();
}

size_t BGE::TextureService::totalHandleMemory() const {
    return textureHandleService_.totalMemory() + textureAtlasHandleService_.totalMemory();
}

size_t BGE::TextureService::totalTextureMemory() const {
    std::vector<TextureHandle> textures;
    size_t usage = 0;
    auto packageService = Game::getInstance()->getScenePackageService();
    auto spaceService = Game::getInstance()->getSpaceService();
    
    for (auto const &packageIt : packageTextures_) {
        auto package = packageService->getScenePackage(packageIt.first);
        
        if (package) {
            for (auto &texIt : packageIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &spaceIt : spaceTextures_) {
        auto space = spaceService->getSpace(spaceIt.first);
        
        if (space) {
            for (auto &texIt : spaceIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &atlasIt : atlasTextures_) {
        auto atlas = getTextureAtlas(atlasIt.first);
        
        if (atlas) {
            for (auto &texIt : atlasIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto handle : textures) {
        auto tex = getTexture(handle);
        
        usage += tex->getMemoryUsage();
    }
    
    return usage;
}

void BGE::TextureService::outputMemoryBreakdown(uint32_t numTabs) const {
    std::vector<TextureHandle> textures;
    auto packageService = Game::getInstance()->getScenePackageService();
    auto spaceService = Game::getInstance()->getSpaceService();
    
    for (auto const &packageIt : packageTextures_) {
        auto package = packageService->getScenePackage(packageIt.first);
        
        if (package) {
            for (auto &texIt : packageIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &spaceIt : spaceTextures_) {
        auto space = spaceService->getSpace(spaceIt.first);
        
        if (space) {
            for (auto &texIt : spaceIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto const &atlasIt : atlasTextures_) {
        auto atlas = getTextureAtlas(atlasIt.first);
        
        if (atlas) {
            for (auto &texIt : atlasIt.second) {
                auto tex = getTexture(texIt.second);
                
                if (tex) {
                    if (std::find(textures.begin(), textures.end(), texIt.second) == textures.end()) {
                        textures.push_back(texIt.second);
                    }
                }
            }
        }
    }
    
    for (auto handle : textures) {
        auto tex = getTexture(handle);

        if (tex && !tex->isSubTexture()) {
            Game::outputValue(numTabs, "%s: %zd bytes\n", tex->getName().c_str(), tex->getMemoryUsage());
        }
    }
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(ScenePackageHandle scenePackageHandle, std::string name) const {
    auto scenePackage = packageTextures_.find(scenePackageHandle);
    
    if (scenePackage != packageTextures_.end()) {
        auto packageTex = scenePackage->second.find(name);
        
        if (packageTex != scenePackage->second.end()) {
            return packageTex->second;
        }
    }
    
    return TextureHandle();
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(SpaceHandle spaceHandle, std::string name) const {
    auto space = spaceTextures_.find(spaceHandle);
    
    if (space != spaceTextures_.end()) {
        auto spaceTex = space->second.find(name);
        
        if (spaceTex != space->second.end()) {
            return spaceTex->second;
        }
    }
    
    return TextureHandle();
}

BGE::TextureHandle BGE::TextureService::getTextureHandle(TextureAtlasHandle atlasHandle, std::string name) const {
    auto atlas = atlasTextures_.find(atlasHandle);
    
    if (atlas != atlasTextures_.end()) {
        auto atlasTex = atlas->second.find(name);
        
        if (atlasTex != atlas->second.end()) {
            return atlasTex->second;
        }
    }
    
    return TextureHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(ScenePackageHandle scenePackageHandle, std::string name) const {
    auto scenePackage = packageTextureAtlases_.find(scenePackageHandle);
    
    if (scenePackage != packageTextureAtlases_.end()) {
        auto packageTex = scenePackage->second.find(name);
        
        if (packageTex != scenePackage->second.end()) {
            return packageTex->second;
        }
    }
    
    return TextureAtlasHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(SpaceHandle spaceHandle, std::string name) const {
    auto space = spaceTextureAtlases_.find(spaceHandle);
    
    if (space != spaceTextureAtlases_.end()) {
        auto spaceTex = space->second.find(name);
        
        if (spaceTex != space->second.end()) {
            return spaceTex->second;
        }
    }
    
    return TextureAtlasHandle();
}

BGE::TextureAtlasHandle BGE::TextureService::getTextureAtlasHandle(FontHandle fontHandle, std::string name) const {
    auto font = fontTextureAtlases_.find(fontHandle);
    
    if (font != fontTextureAtlases_.end()) {
        auto fontTex = font->second.find(name);
        
        if (fontTex != font->second.end()) {
            return fontTex->second;
        }
    }
    
    return TextureAtlasHandle();
}

BGE::Texture *BGE::TextureService::getTexture(ScenePackageHandle scenePackageHandle, std::string name) const {
    return textureHandleService_.dereference(getTextureHandle(scenePackageHandle, name));
}

BGE::Texture *BGE::TextureService::getTexture(SpaceHandle spaceHandle, std::string name) const {
    return textureHandleService_.dereference(getTextureHandle(spaceHandle, name));
}

BGE::Texture *BGE::TextureService::getTexture(TextureAtlasHandle atlasHandle, std::string name) const {
    return textureHandleService_.dereference(getTextureHandle(atlasHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(ScenePackageHandle scenePackageHandle, std::string name) const {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(scenePackageHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(SpaceHandle spaceHandle, std::string name) const {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(spaceHandle, name));
}

BGE::TextureAtlas *BGE::TextureService::getTextureAtlas(FontHandle fontHandle, std::string name) const {
    return textureAtlasHandleService_.dereference(getTextureAtlasHandle(fontHandle, name));
}

void BGE::TextureService::removeTexture(ScenePackageHandle scenePackageHandle, TextureHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto scenePackage = packageTextures_.find(scenePackageHandle);
    
    if (scenePackage != packageTextures_.end()) {
        for (auto const &packageTex : scenePackage->second) {
            if (packageTex.second == handle) {
                auto texture = getTexture(handle);
                releaseTexture(texture);
                
                scenePackage->second.erase(packageTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTexture(SpaceHandle spaceHandle, TextureHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto space = spaceTextures_.find(spaceHandle);
    
    if (space != spaceTextures_.end()) {
        for (auto const &spaceTex : space->second) {
            if (spaceTex.second == handle) {
                auto texture = getTexture(handle);
                releaseTexture(texture);

                space->second.erase(spaceTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTexture(TextureAtlasHandle atlasHandle, TextureHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto atlas = atlasTextures_.find(atlasHandle);
    
    if (atlas != atlasTextures_.end()) {
        for (auto const &atlasTex : atlas->second) {
            if (atlasTex.second == handle) {
                auto texture = getTexture(handle);
                releaseTexture(texture);
                
                atlas->second.erase(atlasTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(ScenePackageHandle scenePackageHandle, TextureAtlasHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto scenePackage = packageTextureAtlases_.find(scenePackageHandle);
    
    if (scenePackage != packageTextureAtlases_.end()) {
        for (auto const &packageTex : scenePackage->second) {
            if (packageTex.second == handle) {
                auto atlas = getTextureAtlas(handle);
                
                // Now remove all subtextures
                auto subTextures = atlas->getSubTextures();
                
                for (auto &subTex : subTextures) {
                    removeTexture(scenePackageHandle, subTex.second);
                }
                
                releaseTextureAtlas(atlas);

                scenePackage->second.erase(packageTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(SpaceHandle spaceHandle, TextureAtlasHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto space = spaceTextureAtlases_.find(spaceHandle);
    
    if (space != spaceTextureAtlases_.end()) {
        for (auto const &spaceTex : space->second) {
            if (spaceTex.second == handle) {
                auto atlas = getTextureAtlas(handle);
                releaseTextureAtlas(atlas);
                
                space->second.erase(spaceTex.first);
                return;
            }
        }
    }
}

void BGE::TextureService::removeTextureAtlas(FontHandle fontHandle, TextureAtlasHandle handle) {
    if (handle.isNull()) {
        return;
    }

    auto font = fontTextureAtlases_.find(fontHandle);
    
    if (font != fontTextureAtlases_.end()) {
        for (auto const &fontTex : font->second) {
            if (fontTex.second == handle) {
                auto atlas = getTextureAtlas(handle);
                releaseTextureAtlas(atlas);
                
                font->second.erase(fontTex.first);
                return;
            }
        }
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, TextureFormat format) {
    auto &scenePackage = packageTextures_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromFile(name, filename, format);
        if (texture) {
            auto &scenePackage = packageTextures_[scenePackageHandle];
            scenePackage[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height) {
    auto &scenePackage = packageTextures_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromBuffer(name, buffer, format, width, height);
        if (texture) {
            auto &scenePackage = packageTextures_[scenePackageHandle];
            scenePackage[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, TextureFormat format) {
    auto &space = spaceTextures_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromFile(name, filename, format);
        if (texture) {
            auto &space = spaceTextures_[spaceHandle];
            space[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height) {
    auto &space = spaceTextures_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromBuffer(name, buffer, format, width, height);
        if (texture) {
            auto &space = spaceTextures_[spaceHandle];
            space[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromFile(TextureAtlasHandle atlasHandle, std::string name, std::string filename, TextureFormat format) {
    auto &atlas = atlasTextures_[atlasHandle];
    auto tex = atlas.find(name);
    
    if (tex == atlas.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromFile(name, filename, format);
        if (texture) {
            auto &atlas = atlasTextures_[atlasHandle];
            atlas[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromBuffer(TextureAtlasHandle atlasHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height) {
    auto &atlas = atlasTextures_[atlasHandle];
    auto tex = atlas.find(name);
    
    if (tex == atlas.end()) {
        Texture *texture;
        std::shared_ptr<Error> error;
        std::tie(texture, error) = createTextureFromBuffer(name, buffer, format, width, height);
        if (texture) {
            auto &atlas = atlasTextures_[atlasHandle];
            atlas[name] = texture->getHandle();
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromFile(std::string name, std::string filename, TextureFormat format)
{
#ifdef SUPPORT_GLKTEXTURELOADER
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_enter(group);
    __block std::shared_ptr<Error> bgeError;
    __block Texture *texture = nullptr;
    [textureLoader_ textureWithContentsOfFile:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding] options:@{ GLKTextureLoaderApplyPremultiplication: @(NO) } queue:nil completionHandler:^(GLKTextureInfo *textureInfo, NSError *error) {
        if (textureInfo) {
            TextureHandle textureHandle;

            texture = textureHandleService_.allocate(textureHandle);
            
            if (texture) {
                texture->initialize(textureHandle, name, format, textureInfo);
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
        dispatch_group_leave(group);
    }];
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
#else
    auto rawTex = RawTexture::createFromPng(filename);

    if (rawTex) {
        TextureHandle textureHandle;
        Texture *texture;
        std::shared_ptr<Error> error;

        texture = textureHandleService_.allocate(textureHandle);
        
        if (texture) {
            error = texture->initialize(textureHandle, name, rawTex);
            if (error) {
                // We had a problem, release the handle
                textureHandleService_.release(textureHandle);
                texture = nullptr;
            }
        } else {
            // We had a problem, release the handle
            textureHandleService_.release(textureHandle);
            error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorOS);
        }
        delete rawTex;
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(nullptr, std::make_shared<Error>(Texture::ErrorDomain, TextureErrorOS));
    }
#endif
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height)
{
    TextureHandle textureHandle;
    Texture *texture = nullptr;
    std::shared_ptr<Error> error;
    if (buffer) {
        texture = textureHandleService_.allocate(textureHandle);
        if (texture) {
            texture->initialize(textureHandle, name, format);
            error = texture->createFromBuffer(buffer, format, width, height);
            if (error) {
                textureHandleService_.release(textureHandle);
                texture = nullptr;
            }
        } else {
            error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorClassAllocation);
        }
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorNoBuffer);
    }
    return std::make_pair(texture, error);
}

#if TARGET_OS_IPHONE
std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureFromUIImage(SpaceHandle spaceHandle, const std::string& name, UIImage *image) {
    auto &space = spaceTextures_[spaceHandle];
    auto tex = space.find(name);

    if (tex == space.end()) {
        CGImageRef imageRef = [image CGImage];
        uint32_t width = static_cast<uint32_t>(CGImageGetWidth(imageRef));
        uint32_t height = static_cast<uint32_t>(CGImageGetHeight(imageRef));
        unsigned char* textureData = (unsigned char *)malloc(width * height * 4); // if 4 components per pixel (RGBA)
        Texture *texture = nullptr;
        std::shared_ptr<Error> error;
        if (textureData) {
            // This requires the mainthread thread
            CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
            NSUInteger bytesPerPixel = 4;
            NSUInteger bytesPerRow = bytesPerPixel * width;
            NSUInteger bitsPerComponent = 8;
            CGContextRef context = CGBitmapContextCreate(textureData, width, height,
                                                         bitsPerComponent, bytesPerRow, colorSpace,
                                                         kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);

            CGColorSpaceRelease(colorSpace);
            CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
            CGContextRelease(context);

            std::tie(texture, error) = createTextureFromBuffer(name, textureData, TextureFormat::RGBA8888, width, height);
            if (texture) {
                auto &space = spaceTextures_[spaceHandle];
                space[name] = texture->getHandle();
            }
            free(textureData);
        } else {
            error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorNoBuffer);
        }
        return std::make_pair(texture, error);
    } else {
        return std::make_pair(textureHandleService_.dereference(tex->second), nullptr);
    }
}

#endif /* TARGET_OS_IPHONE */

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromFile(ScenePackageHandle scenePackageHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format) {
    auto &scenePackage = packageTextureAtlases_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromFile(name, filename, subTextureDefs, format);
        if (atlas) {
            auto &scenePackage = packageTextureAtlases_[scenePackageHandle];
            scenePackage[name] = atlas->getHandle();

            // Now for each item in the atlas, add it to normal textures
            auto &sceneTex = packageTextures_[scenePackageHandle];
            auto subTextures = atlas->getSubTextures();

            sceneTex.insert(subTextures.begin(), subTextures.end());
        }

        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromBuffer(ScenePackageHandle scenePackageHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs) {
    auto &scenePackage = packageTextureAtlases_[scenePackageHandle];
    auto tex = scenePackage.find(name);
    
    if (tex == scenePackage.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs);
        if (atlas) {
            auto &scenePackage = packageTextureAtlases_[scenePackageHandle];
            scenePackage[name] = atlas->getHandle();
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromFile(SpaceHandle spaceHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format) {
    auto &space = spaceTextureAtlases_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromFile(name, filename, subTextureDefs, format);
        if (atlas) {
            auto &space = spaceTextureAtlases_[spaceHandle];
            space[name] = atlas->getHandle();
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromBuffer(SpaceHandle spaceHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs) {
    auto &space = spaceTextureAtlases_[spaceHandle];
    auto tex = space.find(name);
    
    if (tex == space.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs);
        if (atlas) {
            auto &space = spaceTextureAtlases_[spaceHandle];
            space[name] = atlas->getHandle();
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromFile(FontHandle fontHandle, std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format) {
    auto &font = fontTextureAtlases_[fontHandle];
    auto tex = font.find(name);
    
    if (tex == font.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromFile(name, filename, subTextureDefs, format);
        if (atlas) {
            auto &font = fontTextureAtlases_[fontHandle];
            font[name] = atlas->getHandle();
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromBuffer(FontHandle fontHandle, std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs) {
    auto &font = fontTextureAtlases_[fontHandle];
    auto tex = font.find(name);
    
    if (tex == font.end()) {
        TextureAtlas *atlas;
        std::shared_ptr<Error> error;
        std::tie(atlas, error) = createTextureAtlasFromBuffer(name, buffer, format, width, height, subTextureDefs);
        if (atlas) {
            auto &font = fontTextureAtlases_[fontHandle];
            font[name] = atlas->getHandle();
        }
        return std::make_pair(atlas, error);
    } else {
        return std::make_pair(textureAtlasHandleService_.dereference(tex->second), nullptr);
    }
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format) {
    std::shared_ptr<Error> error;
    TextureAtlasHandle atlasHandle;
    TextureAtlas *atlas;
    atlas = textureAtlasHandleService_.allocate(atlasHandle);
    
    if (atlas) {
        atlas->initialize(atlasHandle, name);
        std::tie(atlas, error) = atlas->createFromFile(filename, subTextureDefs, format);
        if (!atlas) {
            // We had a problem, release the handle
            textureAtlasHandleService_.release(atlasHandle);
        }
    } else {
        error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorClassAllocation);
    }
    return std::make_pair(atlas, error);
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::TextureService::createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs) {
    std::shared_ptr<Error> error;
    TextureAtlasHandle atlasHandle;
    TextureAtlas *atlas;
    atlas = textureAtlasHandleService_.allocate(atlasHandle);
    if (atlas) {
        atlas->initialize(atlasHandle, name);
        
        std::tie(atlas, error) = atlas->createFromBuffer(buffer, format, width, height, subTextureDefs);
        if (!atlas) {
            // We had a problem, release the handle
            textureAtlasHandleService_.release(atlasHandle);
        }
    } else {
        error = std::make_shared<Error>(TextureAtlas::ErrorDomain, TextureErrorClassAllocation);
    }
    return std::make_pair(atlas, error);
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
        texture->initialize(textureHandle, name, atlas->getFormat());

        auto error = texture->createSubTexture(atlas, x, y, width, height, rotated);
        
        if (error) {
            releaseTexture(texture);
            texture = nullptr;
        }
    }
    
    return texture;
}


void BGE::TextureService::releaseTexture(Texture *texture) {
    if (texture) {
        auto textureHandle = texture->getHandle();
        
        texture->destroy();
        textureHandleService_.release(textureHandle);
    }
}

void BGE::TextureService::releaseTextureAtlas(TextureAtlas *atlas) {
    if (atlas) {
        auto atlasHandle = atlas->getHandle();
        
        atlas->destroy();
        textureAtlasHandleService_.release(atlasHandle);
    }
}

