//
//  SpaceService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpaceService.h"
#include "Game.h"

BGE::SpaceService::SpaceService() : handleService_(InitialSpaceReserve, HandleServiceNoMaxLimit){
}

uint32_t BGE::SpaceService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::SpaceService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::SpaceService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::SpaceService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

size_t BGE::SpaceService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::SpaceService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::SpaceService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

uint32_t BGE::SpaceService::numSpaces() const {
    uint32_t num = 0;
    
    for (auto const &handle : spaces_) {
        auto space = getSpace(handle);
        
        if (space) {
            num++;
        } else {
            assert(false);
        }
    }
    
    return num;
}

void BGE::SpaceService::outputResourceBreakdown(uint32_t numTabs) const {
    for (auto const &handle : spaces_) {
        auto space = getSpace(handle);
        
        if (space) {
            space->outputResourceBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

void BGE::SpaceService::outputMemoryBreakdown(uint32_t numTabs) const {
    for (auto const &handle : spaces_) {
        auto space = getSpace(handle);
        
        if (space) {
            space->outputMemoryBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

BGE::Space *BGE::SpaceService::createSpace(std::string name) {
    SpaceHandle handle;
    Space *space = handleService_.allocate(handle);
    
    if (space) {
        space->initialize(handle, name);
        spaces_.push_back(handle);
    }
    
    return space;

}

void BGE::SpaceService::removeSpace(SpaceHandle spaceHandle) {
    for (auto it=spaces_.begin(); it!=spaces_.end(); it++) {
        if (*it == spaceHandle) {
            auto space = getSpace(*it);
            
            if (space) {
                space->destroy();
            }
            
            spaces_.erase(it);
            return;
        }
    }
}

void BGE::SpaceService::removeSpace(std::string name) {
    for (auto const &handle : spaces_) {
        auto space = getSpace(handle);
        
        if (space) {
            if (space->getName() == name) {
                removeSpace(space->getHandle());
                return;
            }
        }
    }
}

BGE::Space *BGE::SpaceService::getSpace(SpaceHandle spaceHandle) const {
    return handleService_.dereference(spaceHandle);
}

BGE::Space *BGE::SpaceService::getSpace(std::string name) const {
    for (auto const &handle : spaces_) {
        auto space = getSpace(handle);
        
        if (space) {
            if (space->getName() == name) {
                return space;
            }
        }
    }
    
    return nullptr;
}

std::vector<BGE::SpaceHandle> BGE::SpaceService::getSpaces() const {
    std::vector<SpaceHandle> spaces;
    
    for(auto handle : spaces_) {
        spaces.push_back(handle);
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
