//
//  Component.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Component.h"
#include "Space.h"
#include "Game.h"

uint32_t BGE::Component::InvalidBitmask = (uint32_t)-1;
uint32_t BGE::Component::InvalidTypeId = (uint32_t)-1;
uint32_t BGE::Component::bitmask_ = InvalidBitmask;
BGE::ComponentTypeId BGE::Component::typeId_ = InvalidTypeId;
std::type_index BGE::Component::type_index_ = typeid(BGE::Component);

void BGE::Component::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    handle_ = handle;
    spaceHandle_ = spaceHandle;
    gameObjectHandle_ = GameObjectHandle();
}

void BGE::Component::destroy() {
    handle_ = 0;
    gameObjectHandle_ = GameObjectHandle();
    spaceHandle_ = SpaceHandle();
}

void BGE::Component::destroyFast() {
    handle_ = 0;
    gameObjectHandle_ = GameObjectHandle();
    spaceHandle_ = SpaceHandle();
}

BGE::Space *BGE::Component::getSpace() const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}

BGE::GameObject *BGE::Component::getGameObject() const {
    auto space = getSpace();
    
    if (space) {
        return space->getGameObject(gameObjectHandle_);
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Component::getGameObjectLockless() const {
    auto space = getSpace();
    return getGameObjectLockless(space);
}

BGE::GameObject *BGE::Component::getGameObject(const Space *space) const {
    return space->getGameObject(gameObjectHandle_);
}

BGE::GameObject *BGE::Component::getGameObjectLockless(const Space *space) const {
    return space->getGameObjectLockless(gameObjectHandle_);
}
