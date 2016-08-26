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

BGE::ScenePackageService::ScenePackageService() : handleService_(InitialScenePackageReserve, HandleServiceNoMaxLimit) {
}

void BGE::ScenePackageService::packageFromJSONFile(SpaceHandle spaceHandle, std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<Error>)> callback) {
    for (auto &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getName() == name) {
            // Check if our spaceHandle is in the refernce list, if not add it
            bool found = false;
            
            for (auto const &ref : packageRef.references) {
                if (ref == spaceHandle) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                packageRef.references.push_back(spaceHandle);
            }
            
            if (callback) {
                callback(packageRef.handle, nullptr);
            }
            
            return;
        }
    }
    
    // Does not exist, to load
    // For now we are doing this via iOS methods for faster dev
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSFileManager *defaultMgr = [NSFileManager defaultManager];
        ScenePackageHandle handle;
        ScenePackage *package = handleService_.allocate(handle);
        
        if (package) {
            NSData *data = [defaultMgr contentsAtPath:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding]];
            NSError *err = nil;
            NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&err];
            
            // The loading system relies on having the package initalized with handle
            package->initialize(handle, name);
            
            package->load(jsonDict, [this, spaceHandle, handle, name, callback](ScenePackage * package) {
                auto tHandle = handle;
                
                if (package) {
#if DEBUG
                    auto found = false;
                    
                    for (auto const &packageRef : scenePackages_) {
                        if (packageRef.handle == tHandle) {
                            found = true;
                        }
                    }
                    
                    assert(!found);
#endif
                    ScenePackageReference ref{ tHandle, { spaceHandle }};
                    
                    scenePackages_.push_back(ref);
                    
                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                    
                    if (space) {
                        space->scenePackageAdded(tHandle);
                    }
                } else {
                    // There is no package, so we need to release this
                    handleService_.release(tHandle);
                    
                    // Reset handle
                    tHandle.nullify();
                }
                
                if (callback) {
                    callback(tHandle, nullptr);
                }
            });
        } else {
            if (callback) {
                callback(handle, nullptr);
            }
        }
    });
}

BGE::ScenePackageHandle BGE::ScenePackageService::getScenePackageHandle(ObjectId scenePackageId) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getInstanceId() == scenePackageId) {
            return package->getHandle();
        }
    }
    
    return ScenePackageHandle();
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

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ObjectId scenePackageId)const  {
    auto handle = getScenePackageHandle(scenePackageId);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(std::string name) const {
    auto handle = getScenePackageHandle(name);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ScenePackageHandle handle) const {
    return handleService_.dereference(handle);
}

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, ObjectId scenePackageId) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        auto package = getScenePackage(it->handle);
        
        if (package && package->getInstanceId() == scenePackageId) {
            auto &refs = it->references;
            
            for (auto hIt=refs.begin(); hIt!=refs.end();++hIt) {
                if (*hIt == spaceHandle) {
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

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, std::string name) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        auto package = getScenePackage(it->handle);
        
        if (package && package->getName() == name) {
            auto &refs = it->references;
            
            for (auto hIt=refs.begin(); hIt!=refs.end();++hIt) {
                if (*hIt == spaceHandle) {
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

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, ScenePackageHandle handle) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        if (it->handle == handle) {
            auto package = getScenePackage(handle);
            auto &refs = it->references;
            
            for (auto hIt=refs.begin(); hIt!=refs.end();++hIt) {
                if (*hIt == spaceHandle) {
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


