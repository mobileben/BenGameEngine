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

void BGE::ScenePackageService::createPackage(SpaceHandle spaceHandle, std::string name, std::string filename, ScenePackageLoadCompletionHandler callback) {
    ScenePackageLoadItem loadable{spaceHandle, name, filename, callback};
    
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
    
    auto found = loadable.filename.find_last_of('.');
    
    if (found != std::string::npos) {
        auto ext = loadable.filename.substr(found + 1);
        
        std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);
        
        if (ext == "json") {
            createPackageFromJSON(loadable, callback);
        } else if (ext == "spkg") {
            createPackageFromSPKG(loadable, callback);
        } else {
            assert(false);
            
            if (callback) {
                callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::UnsupportedFormat)));
            }
        }
    } else {
        assert(false);
        
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::UnsupportedFormat)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromJSON(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    // Does not exist, to load
    // For now we are doing this via iOS methods for faster dev
    NSFileManager *defaultMgr = [NSFileManager defaultManager];
    ScenePackageHandle handle;
    ScenePackage *package = handleService_.allocate(handle);
    
    if (package) {
        package->initialize(handle, loadable.name);
        package->setStatus(ScenePackageStatus::Loading);
        
        // Now create an entry for this
        ScenePackageReference ref{ handle, { loadable.spaceHandle }};
        scenePackages_.push_back(ref);
        
        NSData *data = [defaultMgr contentsAtPath:[[NSString alloc] initWithCString:loadable.filename.c_str() encoding:NSUTF8StringEncoding]];
        NSError *err = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&err];
        
        // The loading system relies on having the package initalized with handle
        package->create(jsonDict, [this, loadable, callback, handle](ScenePackage * package) {
            auto tHandle = handle;
            std::shared_ptr<Error> error = nullptr;
            
            if (package) {
                package->setStatus(ScenePackageStatus::Valid);

                auto space = Game::getInstance()->getSpaceService()->getSpace(loadable.spaceHandle);
                
                if (space) {
                    space->scenePackageAdded(tHandle);
                }
            } else {
                error = std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::Loading));

                // There is no package, so we need to release this
                removePackage(loadable.spaceHandle, tHandle);
                
                // Reset handle
                tHandle.nullify();
            }
            
            if (callback) {
                callback(tHandle, error);
            }
        });
    } else {
        if (callback) {
            callback(handle, nullptr);
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
    std::mutex                      mutex;
    std::condition_variable         cond;
    std::unique_lock<std::mutex>    lck(mutex);
    
    while (true) {
        printf("XXXX: loadThreadFunction Top 'o loop\n");
        
        auto loadable = queuedLoadItems_.pop();
        
        printf("XXXX: Processing item %s\n", loadable.name.c_str());
        auto f = std::async(std::launch::async, static_cast<void(ScenePackageService::*)(ScenePackageLoadItem, ScenePackageLoadCompletionHandler)>(&ScenePackageService::createPackage), this, loadable, [&cond, loadable](ScenePackageHandle packageHandle, std::shared_ptr<Error> error) {
            if (loadable.completionHandler) {
                loadable.completionHandler(packageHandle, error);
            }
            cond.notify_all();
        });
        
        cond.wait(lck);
        
        printf("BITCHY\n");
    }
    
}


