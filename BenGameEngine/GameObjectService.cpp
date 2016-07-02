//
//  GameObjectService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GameObjectService.h"
#include "GameObject.h"
#include "Game.h"

BGE::GameObjectService::GameObjectService() {
}

BGE::GameObjectService::~GameObjectService() {
}

void BGE::GameObjectService::removeObject(std::shared_ptr<GameObject> object) {
    if (object) {
        removeObject(object->getInstanceId());
    }
}

void BGE::GameObjectService::removeObject(ObjectId objId) {
    GameObjectMap::iterator it = objects_.find(objId);
    
    if (it != objects_.end()) {
        objects_.erase(objId);
    }
}

void BGE::GameObjectService::removeObject(std::string name) {
    for (auto obj : objects_) {
        if (obj.second->getName() == name) {
            removeObject(obj.second->getInstanceId());
            return;
        }
    }
}

std::shared_ptr<BGE::GameObject> BGE::GameObjectService::find(std::shared_ptr<GameObject> object) {
    if (object) {
        return find(object->getInstanceId());
    } else {
        return std::shared_ptr<GameObject>();
    }
}

std::shared_ptr<BGE::GameObject> BGE::GameObjectService::find(ObjectId objId) {
    GameObjectMap::iterator it = objects_.find(objId);
    
    return it->second;
}

std::shared_ptr<BGE::GameObject> BGE::GameObjectService::find(std::string name) {
    std::shared_ptr<GameObject> found;
    
    for (auto kv : objects_) {
        if (kv.second->getName() == name) {
            found = kv.second;
            break;
        }
    }
    
    return found;
}

BGE::Space *BGE::GameObjectService::getSpace(void) const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}
