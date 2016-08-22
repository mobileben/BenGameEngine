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

void BGE::ScenePackageService::packageFromJSONFile(std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<Error>)> callback) {
    for (auto it : scenePackages_) {
        auto package = getScenePackage(it.first);
        
        if (package && package->getName() == name) {
            if (callback) {
                callback(it.second, nullptr);
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
            
            package->load(jsonDict, [this, handle, name, callback](ScenePackage * package) {
                auto tHandle = handle;
                
                if (package) {
                    scenePackages_[package->getInstanceId()] = tHandle;
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

BGE::ScenePackageHandle BGE::ScenePackageService::getScenePackageHandle(ObjectId scenePackageId) {
    auto it = scenePackages_.find(scenePackageId);
    
    if (it != scenePackages_.end()) {
        return it->second;
    } else {
        return ScenePackageHandle();
    }
}

BGE::ScenePackageHandle BGE::ScenePackageService::getScenePackageHandle(std::string name) {
    for (auto it : scenePackages_) {
        auto package = getScenePackage(it.second);
        
        if (package) {
            if (package->getName() == name) {
                return it.second;
            }
        }
    }
    
    return ScenePackageHandle();
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ObjectId scenePackageId) {
    auto handle = getScenePackageHandle(scenePackageId);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(std::string name) {
    auto handle = getScenePackageHandle(name);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ScenePackageHandle handle) {
    return handleService_.dereference(handle);
}

void BGE::ScenePackageService::removePackage(ObjectId scenePackageId) {
    auto it = scenePackages_.find(scenePackageId);
    
    if (it != scenePackages_.end()) {
        handleService_.release(it->second);
        scenePackages_.erase(it->first);
    }
}

void BGE::ScenePackageService::removePackage(std::string name) {
    for (auto it : scenePackages_) {
        auto package = getScenePackage(it.second);
        
        if (package) {
            if (package->getName() == name) {
                removePackage(it.first);
                return;
            }
        }
    }
}

void BGE::ScenePackageService::removePackage(ScenePackageHandle handle) {
    for (auto it : scenePackages_) {
        if (it.second == handle) {
            removePackage(it.first);
            return;
        }
    }
}

void BGE::ScenePackageService::resetPackage(std::string name) {
}

void BGE::ScenePackageService::resetPackage(ScenePackageHandle handle) {
    
}


void BGE::ScenePackageService::link() {
    // TODO: Build dependency lists
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            package->link();
        }
    }
}

BGE::ButtonReference *BGE::ScenePackageService::getButtonReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto button = package->getButtonReference(name);
            
            if (button) {
                return button;
            }
        }
    }
    
    return nullptr;
}

BGE::MaskReference *BGE::ScenePackageService::getMaskReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto mask = package->getMaskReference(name);
            
            if (mask) {
                return mask;
            }
        }
    }
    
    return nullptr;
}

BGE::PlacementReference *BGE::ScenePackageService::getPlacementReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto placement = package->getPlacementReference(name);
            
            if (placement) {
                return placement;
            }
        }
    }
    
    return nullptr;
}

BGE::TextureReference *BGE::ScenePackageService::getTextureReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto texture = package->getTextureReference(name);
            
            if (texture) {
                return texture;
            }
        }
    }
    
    return nullptr;
}

BGE::TextReference *BGE::ScenePackageService::getTextReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto text = package->getTextReference(name);
            
            if (text) {
                return text;
            }
        }
    }
    
    return nullptr;
}

BGE::AnimationSequenceReference *BGE::ScenePackageService::getAnimationSequenceReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto animSeq = package->getAnimationSequenceReference(name);
            
            if (animSeq) {
                return animSeq;
            }
        }
    }

    return nullptr;
}

BGE::ExternalPackageReference *BGE::ScenePackageService::getExternalReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto extRef = package->getExternalReference(name);
            
            if (extRef) {
                return extRef;
            }
        }
    }
    
    return nullptr;
}

BGE::GfxReferenceType BGE::ScenePackageService::getReferenceType(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            auto type = package->getReferenceType(name);
            
            if (type != GfxReferenceTypeUnknown) {
                return type;
            }
        }
    }

    return GfxReferenceTypeUnknown;
}


