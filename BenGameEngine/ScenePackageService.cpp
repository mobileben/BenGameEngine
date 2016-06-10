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

BGE::ScenePackageService::ScenePackageService() : scenePackageHandleService_(InitialScenePackageReserve, ScenePackageHandleService::NoMaxLimit) {
}

void BGE::ScenePackageService::packageFromJSONFile(std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<BGE::Error>)> callback) {
    ScenePackagesMapIterator it = scenePackages_.find(name);
    
    if (it != scenePackages_.end()) {
        if (callback) {
            callback(it->second, nullptr);
        }
    } else {
        // Does not exist, to load
        // For now we are doing this via iOS methods for faster dev
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            NSFileManager *defaultMgr = [NSFileManager defaultManager];
            ScenePackageHandle handle;
            ScenePackage *package = scenePackageHandleService_.allocate(handle);
            
            if (package) {
                NSData *data = [defaultMgr contentsAtPath:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding]];
                NSError *err = nil;
                NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&err];

                package->load(jsonDict, [this, handle, callback](ScenePackage * package) {
                    if (package) {
                        scenePackages_[package->getName()] = handle;
                    }
                    
                    if (callback) {
                        callback(handle, nullptr);
                    }
                });
            } else {
                if (callback) {
                    callback(handle, nullptr);
                }
            }
        });
    }
}

BGE::ScenePackageHandle BGE::ScenePackageService::getPackage(std::string name) {
    auto it = scenePackages_.find(name);
    
    if (it != scenePackages_.end()) {
        return it->second;
    }
    
    return ScenePackageHandle();
}

BGE::ScenePackage *BGE::ScenePackageService::getDereferencedPackage(std::string name) {
    ScenePackageHandle handle = getPackage(name);
    
    return scenePackageHandleService_.dereference(handle);
}

BGE::ScenePackage * BGE::ScenePackageService::getDereferencedPackage(ScenePackageHandle handle) {
    return scenePackageHandleService_.dereference(handle);
}

void BGE::ScenePackageService::deletePackage(std::string name) {
}

void BGE::ScenePackageService::deletePackage(ScenePackageHandle handle) {
    
}

void BGE::ScenePackageService::resetPackage(std::string name) {
}

void BGE::ScenePackageService::resetPackage(ScenePackageHandle handle) {
    
}


void BGE::ScenePackageService::link() {
    // TODO: Build dependency lists
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = scenePackageHandleService_.dereference(handle);
        
        if (package) {
            package->link();
        }
    }
}

std::shared_ptr<BGE::SpriteRenderComponent> BGE::ScenePackageService::createSpriteRenderComponent(std::string name) {
    for (auto const &ent : scenePackages_) {
        ScenePackageHandle handle = ent.second;
        auto package = scenePackageHandleService_.dereference(handle);
        
        if (package) {
            TextureReference *texRef = package->getTextureReference(name);
            
            if (texRef && texRef->texture) {
                auto material = BGE::Game::getInstance()->getMaterialService()->createMaterial("mat", texRef->texture);
                auto sprite = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::SpriteRenderComponent>("sprite");
                
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
        auto package = scenePackageHandleService_.dereference(handle);
        
        if (package) {
            TextReference *textRef = package->getTextReference(name);
            
            if (textRef && textRef->font) {
                auto text = BGE::Game::getInstance()->getComponentService()->createComponent<BGE::TextComponent>("text");
                
                text->setTextReference(*textRef);
                
                return text;
            }
        }
    }
    
    return nullptr;
}

