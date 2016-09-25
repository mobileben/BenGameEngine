//
//  LogicService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/24/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LogicService.h"
#include "Game.h"
#include "GameObject.h"
#include "LogicComponent.h"

BGE::LogicService::LogicService() :  Service() {
}

void BGE::LogicService::update(double deltaTime) {
    auto spaceService = Game::getInstance()->getSpaceService();
    auto paused = Game::getInstance()->isPaused();
    
    for (auto &item : gameObjectHandles_) {
        auto space = spaceService->getSpace(item.first);
        auto gameObj = space->getGameObject(item.second);
        auto logic = gameObj->getComponent<LogicComponent>();
        
        assert(logic);
        
        if (logic && logic->update) {
            if (logic->mode == LogicComponentMode::Always || (paused && logic->mode == LogicComponentMode::Paused) || logic->mode == LogicComponentMode::Unapaused) {
                logic->update(gameObj, deltaTime);
            }
        }
    }
}

void BGE::LogicService::addGameObject(GameObject *gameObject) {
    std::pair<SpaceHandle, GameObjectHandle> item = std::make_pair(gameObject->getSpace()->getHandle(), gameObject->getHandle());
    
    if (std::find(gameObjectHandles_.begin(), gameObjectHandles_.end(), item) == gameObjectHandles_.end()) {
        gameObjectHandles_.push_back(item);
    }
}

void BGE::LogicService::removeGameObject(GameObject *gameObject) {
    std::pair<SpaceHandle, GameObjectHandle> item = std::make_pair(gameObject->getSpace()->getHandle(), gameObject->getHandle());
    
    auto it = std::find(gameObjectHandles_.begin(), gameObjectHandles_.end(), item);
    if (it != gameObjectHandles_.end()) {
        gameObjectHandles_.erase(it);
    }
}
