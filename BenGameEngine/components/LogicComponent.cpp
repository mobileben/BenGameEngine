//
//  LogicComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/24/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Game.h"
#include "LogicComponent.h"
#include "LogicService.h"

uint32_t BGE::LogicComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::LogicComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::LogicComponent::type_index_ = typeid(BGE::LogicComponent);

BGE::LogicComponent::LogicComponent() : Component(), mode(LogicComponentMode::Always), priority(0) {
}

void BGE::LogicComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    mode = LogicComponentMode::Always;
    priority = 0;
}

void BGE::LogicComponent::destroy() {
    auto gameObject = getGameObject();
    
    if (gameObject) {
        Game::getInstance()->getLogicService()->removeGameObject(gameObject);
    }
    
    this->update = nullptr;
    
    // Component::destroy last
    Component::destroy();
}
