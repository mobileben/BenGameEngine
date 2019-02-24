//
//  SpaceService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpaceService.h"
#include "Game.h"

#if DEBUG
#include <pthread.h>
#endif

BGE::SpaceService::SpaceService() : handleService_(InitialSpaceReserve, HandleServiceNoMaxLimit){
    thread_ = std::thread(&SpaceService::threadFunction, this);
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

BGE::Space *BGE::SpaceService::createSpace(std::string name, uint32_t order) {
    SpaceHandle handle;
    Space *space = handleService_.allocate(handle);
    
    if (space) {
        space->initialize(handle, name, order, shared_from_this());

        // Now insert into proper part of list. This is a sorted list
        bool inserted = false;
        auto length = spaces_.size();
        for (size_t i=0;i<length;++i) {
            auto other = getSpace(spaces_[i]);
            if (space->order_ < other->order_) {
                // Insert
                spaces_.insert(spaces_.begin() + i, handle);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            // Insert into end
            spaces_.push_back(handle);
        }
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

void BGE::SpaceService::queueReset(SpaceHandle spaceHandle) {
    resetQueue_.push(spaceHandle);
}

BGE::Space *BGE::SpaceService::getSpace(SpaceHandle spaceHandle) const {
    return handleService_.dereference(spaceHandle);
}

BGE::Space *BGE::SpaceService::getSpaceLockless(SpaceHandle spaceHandle) const {
    return handleService_.dereferenceLockless(spaceHandle);
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

BGE::Space *BGE::SpaceService::getSpaceLockless(std::string name) const {
    for (auto const &handle : spaces_) {
        auto space = getSpaceLockless(handle);
        
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
    return spaces;
}

void BGE::SpaceService::getSpaces(std::vector<BGE::SpaceHandle>& spaces) const {
    spaces.clear();
    spaces.reserve(spaces_.size());
    
    for(auto handle : spaces_) {
        spaces.push_back(handle);
    }
}

std::vector<BGE::SpaceHandle> BGE::SpaceService::getReversedSpaces() const {
    std::vector<SpaceHandle> spaces;
    
    for(auto it=spaces_.rbegin();it!=spaces_.rend();++it) {
        spaces.push_back(*it);
    }
    return spaces;
}

void BGE::SpaceService::getReversedSpaces(std::vector<BGE::SpaceHandle>& spaces) const {
    spaces.clear();
    spaces.reserve(spaces_.size());
    for(auto it=spaces_.rbegin();it!=spaces_.rend();++it) {
        spaces.push_back(*it);
    }
}

void BGE::SpaceService::reorderSpaces() {
    std::sort(spaces_.begin(), spaces_.end(),
              [this](const SpaceHandle &lhs, const SpaceHandle &rhs) {
                  auto lSpace = this->getSpace(lhs);
                  auto rSpace = this->getSpace(rhs);
                  
                  if (lSpace && rSpace) {
                      return lSpace->order_ < rSpace->order_;
                  }

                  return false;
              });
}

void BGE::SpaceService::threadFunction() {
    auto native = thread_.native_handle();
    if (native == pthread_self()) {
        pthread_setname_np("space");
    }
    while(true) {
        auto handle = resetQueue_.pop();
        auto space = getSpace(handle);
        if (space) {
            space->reset_();
        }
    }
}
