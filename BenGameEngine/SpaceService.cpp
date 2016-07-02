//
//  SpaceService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpaceService.h"
#include "Game.h"

BGE::SpaceService::SpaceService() : spaceHandleService_(InitialSpaceReserve, SpaceHandleService::NoMaxLimit){
}

BGE::SpaceService::~SpaceService() {
}

BGE::SpaceHandle BGE::SpaceService::createSpace(std::string name) {
    SpaceHandle handle;
    Space *space = spaceHandleService_.allocate(handle);
    
    if (space) {
        uint64_t spaceId = getIdAndIncrement();
        
        space->initialize(handle, spaceId, name);
        spaces_[spaceId] = handle;
    }
    
    return handle;

}

void BGE::SpaceService::removeSpace(SpaceHandle spaceHandle) {
    Space *space = getSpace(spaceHandle);
    
    if (space) {
        removeSpace(space->getInstanceId());
    }
}

void BGE::SpaceService::removeSpace(uint64_t objId) {
    auto it = spaces_.find(objId);
    
    if (it != spaces_.end()) {
        spaces_.erase(objId);
    }
}

void BGE::SpaceService::removeSpace(std::string name) {
    for (auto sp : spaces_) {
        auto space = getSpace(sp.second);
        
        if (space) {
            if (space->getName() == name) {
                removeSpace(space->getInstanceId());
                return;
            }
        }
    }
}

BGE::Space *BGE::SpaceService::getSpace(SpaceHandle spaceHandle) {
    return spaceHandleService_.dereference(spaceHandle);
}

BGE::Space *BGE::SpaceService::getSpace(uint64_t objId) {
    auto it = spaces_.find(objId);
    
    if (it != spaces_.end()) {
        return getSpace(it->second);
    }
    
    return nullptr;
}

BGE::Space *BGE::SpaceService::getSpace(std::string name) {
    for (auto sp : spaces_) {
        auto space = getSpace(sp.second);
        
        if (space) {
            if (space->getName() == name) {
                return space;
            }
        }
    }
    
    return nullptr;
}

std::vector<BGE::SpaceHandle> BGE::SpaceService::getSpaces() {
    std::vector<SpaceHandle> spaces;
    
    for(auto kv : spaces_) {
        spaces.push_back(kv.second);
    }

    std::sort(spaces.begin(), spaces.end(),
              [this](const SpaceHandle &lhs, const SpaceHandle &rhs) {
                  auto lSpace = this->getSpace(lhs);
                  auto rSpace = this->getSpace(rhs);
                  
                  if (lSpace && rSpace) {
                      return lSpace->order_ < rSpace->order_;
                  }
                  
                  return false;
              });
    
    return spaces;
}
