//
//  BGEGameObjectService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEGameObjectService.h"
#include "BGEGameObject.h"

BGEGameObjectService::BGEGameObjectService() {
}

BGEGameObjectService::~BGEGameObjectService() {
}

void BGEGameObjectService::removeObject(uint64_t objId) {
    std::unordered_map<uint64_t, std::shared_ptr<BGEGameObject>>::iterator it = objects_.find(objId);
    
    if (it != objects_.end()) {
        objects_.erase(objId);
    }
}

std::shared_ptr<BGEGameObject> BGEGameObjectService::find(uint64_t objId) {
    std::unordered_map<uint64_t, std::shared_ptr<BGEGameObject>>::iterator it = objects_.find(objId);
    
    return it->second;
}

std::shared_ptr<BGEGameObject> BGEGameObjectService::find(std::string name) {
    std::shared_ptr<BGEGameObject> found;
    
    for (auto kv : objects_) {
        if (kv.second->getName() == name) {
            found = kv.second;
            break;
        }
    }
    
    return found;
}
