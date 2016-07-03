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

BGE::ScenePackageService::ScenePackageService() : handleService_(InitialScenePackageReserve, ScenePackageHandleService::NoMaxLimit) {
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
            
            package->load(jsonDict, [this, handle, name, callback](ScenePackage * package) {
                auto tHandle = handle;
                
                if (package) {
                    ObjectId objId = this->getIdAndIncrement();
                    
                    package->initialize(tHandle, objId, name);
                    
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

std::shared_ptr<BGE::SpriteRenderComponent> BGE::ScenePackageService::createSpriteRenderComponent(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            TextureReference *texRef = package->getTextureReference(name);
            
            if (texRef && texRef->texture) {
                auto material = Game::getInstance()->getMaterialService()->createMaterial(texRef->texture);
                auto sprite = Game::getInstance()->getComponentService()->createComponent<SpriteRenderComponent>();
                
                sprite->setMaterials({material});
                
                return sprite;
            }
        }
    }
    
    return nullptr;
}


std::shared_ptr<BGE::TextComponent> BGE::ScenePackageService::createTextComponent(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            TextReference *textRef = package->getTextReference(name);
            
            if (textRef && !textRef->fontHandle.isNull()) {
                auto text = Game::getInstance()->getComponentService()->createComponent<TextComponent>();
                
                text->setTextReference(*textRef);
                
                return text;
            }
        }
    }
    
    return nullptr;
}

std::shared_ptr<BGE::AnimationSequenceComponent> BGE::ScenePackageService::createAnimationSequenceComponent(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            AnimationSequenceReference *animRef = package->getAnimationSequenceReference(name);
            
            if (animRef) {
                auto anim = Game::getInstance()->getComponentService()->createComponent<AnimationSequenceComponent>();
                anim->setAnimationSequenceReference(*animRef);
                
                return anim;
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
            return package->getTextureReference(name);
        }
    }
    
    return nullptr;
}

BGE::TextReference *BGE::ScenePackageService::getTextReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            return package->getTextReference(name);
        }
    }
    
    return nullptr;
}

BGE::AnimationSequenceReference *BGE::ScenePackageService::getAnimationSequenceReference(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = handleService_.dereference(handle);
        
        if (package) {
            return package->getAnimationSequenceReference(name);
        }
    }

    return nullptr;
}


