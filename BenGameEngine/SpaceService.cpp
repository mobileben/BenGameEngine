//
//  SpaceService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpaceService.h"

BGE::SpaceService::SpaceService() {
}

BGE::SpaceService::~SpaceService() {
}

std::shared_ptr<BGE::Space> BGE::SpaceService::createSpace(std::string name) {
    uint64_t spaceId = getIdAndIncrement();
    
    std::shared_ptr<Space> space = Space::create(spaceId, Game::getInstance()->getGameObjectService());
    
    spaces_[spaceId] = space;
    
    return space;

}

void BGE::SpaceService::removeSpace(std::shared_ptr<Space> space) {
    if (space) {
        removeSpace(space->getInstanceId());
    }
}

void BGE::SpaceService::removeSpace(uint64_t objId) {
    std::unordered_map<uint64_t, std::shared_ptr<Space>>::iterator it = spaces_.find(objId);
    
    if (it != spaces_.end()) {
        spaces_.erase(objId);
    }
}

void BGE::SpaceService::removeSpace(std::string name) {
    for (auto space : spaces_) {
        if (space.second->getName() == name) {
            removeSpace(space.second->getInstanceId());
            return;
        }
    }
}

std::shared_ptr<BGE::Space> BGE::SpaceService::find(std::shared_ptr<BGE::Space> space) {
    if (space) {
        return find(space->getInstanceId());
    } else {
        return std::shared_ptr<Space>();
    }
}

std::shared_ptr<BGE::Space> BGE::SpaceService::find(uint64_t spaceId) {
    std::unordered_map<uint64_t, std::shared_ptr<Space>>::iterator it = spaces_.find(spaceId);
    
    return it->second;
}

std::shared_ptr<BGE::Space> BGE::SpaceService::find(std::string name) {
    std::shared_ptr<Space> found;
    
    for (auto kv : spaces_) {
        if (kv.second->getName() == name) {
            found = kv.second;
            break;
        }
    }
    
    return found;
}
