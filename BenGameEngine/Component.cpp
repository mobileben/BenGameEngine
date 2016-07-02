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

std::shared_ptr<BGE::Component> BGE::Component::create(uint64_t componentId) {
    return std::make_shared<Component>(private_key(), componentId);
}

std::shared_ptr<BGE::Component> BGE::Component::create(uint64_t componentId, std::string name) {
    return std::make_shared<Component>(private_key(), componentId, name);
}

BGE::Component::Component(struct private_key const& key, uint64_t componentId) : BGE::Object(componentId) {
}

BGE::Component::Component(struct private_key const& key, uint64_t componentId, std::string name) : BGE::Object(componentId, name) {
}

BGE::Component::Component(uint64_t componentId) : BGE::Object(componentId) {
}

BGE::Component::Component(uint64_t componentId, std::string name) : BGE::Object(componentId, name) {
}

BGE::Space *BGE::Component::getSpace() const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}
