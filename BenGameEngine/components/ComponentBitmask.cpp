//
//  ComponentBitmask.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ComponentBitmask.h"

uint32_t BGE::ComponentBitmask::index_ = 0;
std::vector<BGE::ComponentBitmaskElement> BGE::ComponentBitmask::masks_;
std::unordered_map<std::type_index, std::pair<uint32_t, uint32_t>> BGE::ComponentBitmask::masksOld_;

uint32_t BGE::ComponentBitmask::bitmaskForTypeIndex(std::type_index index) {
    auto it = masksOld_.find(index);
    
    if (it != masksOld_.end()) {
        return it->second.second;
    }
    
    return 0;
}
