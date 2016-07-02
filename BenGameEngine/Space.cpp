//
//  Space.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Space.h"
#include "Game.h"

BGE::Space::Space( uint64_t spaceId) : BGE::Object(spaceId) {
}

BGE::Space::Space(uint64_t spaceId, std::string name) : BGE::Object(spaceId, name) {
}

void BGE::Space::initialize(SpaceHandle handle, uint64_t spaceId, std::string name) {
    spaceHandle_ = handle;
    setInstanceId(spaceId);
    setName(name);

    gameObjectService_ = std::make_shared<GameObjectService>();
    gameObjectService_->setSpaceHandle(spaceHandle_);
    
    componentService_ = std::make_shared<ComponentService>();
    componentService_->setSpaceHandle(spaceHandle_);
}
