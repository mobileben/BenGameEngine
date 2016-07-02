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

std::shared_ptr<BGE::Component> BGE::Component::create(ObjectId componentId) {
    return std::make_shared<Component>(private_key(), componentId);
}

BGE::Component::Component(struct private_key const& key, ObjectId componentId) : Object(componentId) {
}

BGE::Component::Component(ObjectId componentId) : Object(componentId) {
}

BGE::Space *BGE::Component::getSpace() const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}
