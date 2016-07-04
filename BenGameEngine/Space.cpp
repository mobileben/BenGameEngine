//
//  Space.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Space.h"
#include "Game.h"

BGE::Space::Space( ObjectId spaceId) : NamedObject(spaceId), visible_(false), order_(0), updatable_(true), interactable_(true) {
}

BGE::Space::Space(ObjectId spaceId, std::string name) : NamedObject(spaceId, name), visible_(false), order_(0), updatable_(true), interactable_(true) {
}

void BGE::Space::initialize(SpaceHandle handle, ObjectId spaceId, std::string name) {
    spaceHandle_ = handle;
    setInstanceId(spaceId);
    setName(name);

    gameObjectService_ = std::make_shared<GameObjectService>();
    gameObjectService_->setSpaceHandle(spaceHandle_);
    
    componentService_ = std::make_shared<ComponentService>();
    componentService_->setSpaceHandle(spaceHandle_);
}
