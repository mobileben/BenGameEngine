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

BGE::GameObjectService::~GameObjectService() {
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

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(ObjectId objId) {
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

BGE::GameObjectHandle BGE::GameObjectService::getGameObjectHandle(std::string name) {
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

