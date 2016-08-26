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

BGE::GameObjectService::GameObjectHandleService BGE::GameObjectService::handleService_(InitialGameObjectReserve, HandleServiceNoMaxLimit);

BGE::GameObjectService::GameObjectService() : Service() {
}

uint32_t BGE::GameObjectService::numGameObjects () const {
    uint32_t num = 0;
    
    for (auto &handle : gameObjects_) {
        if (handleService_.dereference(handle)) {
            num++;
        }
    }
    
    return num;
}

size_t BGE::GameObjectService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::GameObjectService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::GameObjectService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

BGE::GameObject *BGE::GameObjectService::createGameObject(std::string name) {
    GameObjectHandle handle;
    auto obj = handleService_.allocate(handle);
    
    if (obj) {
        obj->initialize(spaceHandle_, handle, name);
        gameObjects_.push_back(handle);
    }
    
    return obj;
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(ObjectId objId) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObject(handle);
        
        if (obj) {
            if (obj->getInstanceId() == objId) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(std::string name) const {
    for (auto const &handle : gameObjects_) {
        auto obj = getGameObject(handle);
        
        if (obj) {
            if (obj->getName() == name) {
                return handle;
            }
        }
    }
    
    return GameObjectHandle();
}

void BGE::GameObjectService::removeGameObject(GameObjectHandle handle) {
    for (auto it = gameObjects_.begin();it != gameObjects_.end();++it) {
        if (*it == handle) {
            releaseObject(handle);
            gameObjects_.erase(it);
            return;
        }
    }
}

void BGE::GameObjectService::removeAllGameObjects() {
    for (auto const &handle : gameObjects_) {
        releaseObject(handle);
    }
    
    gameObjects_.clear();
}


void BGE::GameObjectService::releaseObject(GameObjectHandle handle) {
    auto obj = getGameObject(handle);

    if (obj) {
        obj->destroy();
        handleService_.release(handle);
    }
}

BGE::Space *BGE::GameObjectService::getSpace(void) const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}

