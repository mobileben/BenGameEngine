//
//  ComponentBitmask.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ComponentBitmask.h"

uint32_t BGE::ComponentBitmask::index_ = 0;
std::unordered_map<std::type_index, uint32_t> BGE::ComponentBitmask::masks_;