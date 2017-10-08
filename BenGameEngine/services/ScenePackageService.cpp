//
//  ScenePackageService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ScenePackageService.h"
#include "Game.h"
#import <Foundation/Foundation.h>
#include <future>
#include <functional>

BGE::ScenePackageService::ScenePackageService() : handleService_(InitialScenePackageReserve, HandleServiceNoMaxLimit) {
    loadThread_ = std::thread(&ScenePackageService::loadThreadFunction, this);
}

uint32_t BGE::ScenePackageService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::ScenePackageService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::ScenePackageService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::ScenePackageService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

size_t BGE::ScenePackageService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::ScenePackageService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::ScenePackageService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

size_t BGE::ScenePackageService::totalMemory() const {
    size_t total = 0;
    
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            total += package->getMemoryUsage();
        }
    }
    
    return total;
}

uint32_t BGE::ScenePackageService::numScenePackages() const {
    uint32_t num = 0;
    
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            num++;
        }
    }
    
    return num;
}

void BGE::ScenePackageService::createPackage(SpaceHandle spaceHandle, std::string name, const FilePath &filePath, ScenePackageLoadCompletionHandler callback) {
    ScenePackageLoadItem loadable{spaceHandle, name, filePath, callback};
    
    queuedLoadItems_.push(loadable);
}

void BGE::ScenePackageService::createPackage(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    for (auto &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getName() == loadable.name) {
            // Check if our spaceHandle is in the refernce list, if not add it
            bool found = false;
            
            for (auto const &ref : packageRef.references) {
                if (ref == loadable.spaceHandle) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                packageRef.references.push_back(loadable.spaceHandle);
            }
            
            if (callback) {
                callback(packageRef.handle, nullptr);
            }
            return;
        }
    }
    
    auto ext = loadable.filePath.extension();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        createPackageFromJSON(loadable, callback);
    } else if (ext == "spkg") {
        createPackageFromSPKG(loadable, callback);
    } else {
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::UnsupportedFormat)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromJSONDict(SpaceHandle spaceHandle, std::string name, NSDictionary *jsonDict, const BaseDirectory &baseDirectory, ScenePackageLoadCompletionHandler callback) {
    if (jsonDict) {
        ScenePackageHandle handle;
        ScenePackage *package = handleService_.allocate(handle);
        
        if (package) {
            package->initialize(handle, name);
            package->setBaseDirectory(baseDirectory);
            package->setStatus(ScenePackageStatus::Loading);
            
            // Now create an entry for this
            ScenePackageReference ref{ handle, { spaceHandle }};
            scenePackages_.push_back(ref);
            
            // The loading system relies on having the package initalized with handle
            package->create(jsonDict, [this, spaceHandle, callback, handle](ScenePackage * package) {
                auto tHandle = handle;
                std::shared_ptr<Error> error = nullptr;
                
                if (package) {
                    package->setStatus(ScenePackageStatus::Valid);
                    
                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                    
                    if (space) {
                        space->scenePackageAdded(tHandle);
                    }
                } else {
                    error = std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::Loading));
                    
                    // There is no package, so we need to release this
                    removePackage(spaceHandle, tHandle);
                    
                    // Reset handle
                    tHandle.nullify();
                }
                
                if (callback) {
                    callback(tHandle, error);
                }
            });
        } else {
            if (callback) {
                callback(handle, std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::NoAvailableHandles)));
            }
        }
    } else {
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromJSON(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    // Does not exist, to load
    // For now we are doing this via iOS methods for faster dev
    NSFileManager *defaultMgr = [NSFileManager defaultManager];
    NSString *filePath = [[NSString alloc] initWithCString:loadable.filePath.filename().c_str() encoding:NSUTF8StringEncoding];
    
    if ([defaultMgr fileExistsAtPath:filePath]) {
        NSData *data = [defaultMgr contentsAtPath:filePath];
        NSError *err = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&err];
        
        createPackageFromJSONDict(loadable.spaceHandle, loadable.name, jsonDict, BaseDirectory{loadable.filePath.type, loadable.filePath.subpath}, callback);
    } else {
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromSPKG(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
}

BGE::ScenePackageHandle BGE::ScenePackageService::getScenePackageHandle(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getName() == name) {
            return package->getHandle();
        }
    }
    
    return ScenePackageHandle();
}


void BGE::ScenePackageService::addSpaceHandleReference(SpaceHandle spaceHandle, ScenePackageHandle packageHandle) {
    for (auto &pkgRef : scenePackages_) {
        if (pkgRef.handle == packageHandle) {
            bool found = false;

            for (auto &ref : pkgRef.references) {
                if (ref == spaceHandle) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                pkgRef.references.push_back(spaceHandle);
                
                auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                
                space->scenePackageAdded(packageHandle);
            }
            break;
        }
    }
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(std::string name) const {
    auto handle = getScenePackageHandle(name);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ScenePackageHandle handle) const {
    return handleService_.dereference(handle);
}

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, std::string name) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        auto package = getScenePackage(it->handle);
        
        if (package && package->getName() == name) {
            removePackage(spaceHandle, it->handle);
        }
    }
}

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, ScenePackageHandle handle) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        if (it->handle == handle) {
            auto package = getScenePackage(handle);
            auto &refs = it->references;
            
            for (auto hIt=refs.begin(); hIt!=refs.end();++hIt) {
                if (*hIt == spaceHandle) {
                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                    
                    if (space) {
                        space->scenePackageRemoved(package->getHandle());
                    }
                    
                    refs.erase(hIt);
                    break;
                }
            }
            
            // We have no more references, so delete the scene
            if (refs.size() == 0) {
                releasePackage(package);
                scenePackages_.erase(it);
            }
            
            break;
        }
    }
}

void BGE::ScenePackageService::resetPackage(std::string name) {
}

void BGE::ScenePackageService::resetPackage(ScenePackageHandle handle) {
    
}

void BGE::ScenePackageService::releasePackage(ScenePackage *package) {
    if (package) {
        auto handle = package->getHandle();
        
        package->destroy();
        handleService_.release(handle);
    }
}

void BGE::ScenePackageService::link() {
    // TODO: Build dependency lists
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);

        if (package) {
            package->link();
        }
    }
}

BGE::ButtonReference *BGE::ScenePackageService::getButtonReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto button = package->getButtonReference(name);
            
            if (button) {
                return button;
            }
        }
    }
    
    return nullptr;
}

BGE::MaskReference *BGE::ScenePackageService::getMaskReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto mask = package->getMaskReference(name);
            
            if (mask) {
                return mask;
            }
        }
    }
    
    return nullptr;
}

BGE::TextureMaskReference *BGE::ScenePackageService::getTextureMaskReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto mask = package->getTextureMaskReference(name);
            
            if (mask) {
                return mask;
            }
        }
    }
    
    return nullptr;
}

BGE::PlacementReference *BGE::ScenePackageService::getPlacementReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto placement = package->getPlacementReference(name);
            
            if (placement) {
                return placement;
            }
        }
    }
    
    return nullptr;
}

BGE::TextureReference *BGE::ScenePackageService::getTextureReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto texture = package->getTextureReference(name);
            
            if (texture) {
                return texture;
            }
        }
    }
    
    return nullptr;
}

BGE::TextReference *BGE::ScenePackageService::getTextReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto text = package->getTextReference(name);
            
            if (text) {
                return text;
            }
        }
    }
    
    return nullptr;
}

BGE::AnimationSequenceReference *BGE::ScenePackageService::getAnimationSequenceReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto animSeq = package->getAnimationSequenceReference(name);
            
            if (animSeq) {
                return animSeq;
            }
        }
    }
    
    return nullptr;
}

BGE::ExternalPackageReference *BGE::ScenePackageService::getExternalReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto extRef = package->getExternalReference(name);
            
            if (extRef) {
                return extRef;
            }
        }
    }
    
    return nullptr;
}

BGE::GfxReferenceType BGE::ScenePackageService::getReferenceType(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto type = package->getReferenceType(name);
            
            if (type != GfxReferenceTypeUnknown) {
                return type;
            }
        }
    }

    return GfxReferenceTypeUnknown;
}

void BGE::ScenePackageService::loadThreadFunction() {
    while (true) {
        auto loadable = queuedLoadItems_.pop();
        
        auto f = std::async(std::launch::async, static_cast<void(ScenePackageService::*)(ScenePackageLoadItem, ScenePackageLoadCompletionHandler)>(&ScenePackageService::createPackage), this, loadable, [loadable](ScenePackageHandle packageHandle, std::shared_ptr<Error> error) {

            if (loadable.completionHandler) {
                loadable.completionHandler(packageHandle, error);
            }
        });
    }
}

void BGE::ScenePackageService::outputResourceBreakdown(uint32_t numTabs) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            package->outputResourceBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

void BGE::ScenePackageService::outputMemoryBreakdown(uint32_t numTabs) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            package->outputMemoryBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

