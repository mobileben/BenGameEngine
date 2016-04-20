//
//  BGEService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEService.h"
#include <cassert>

uint64_t BGEService::getIdAndIncrement() {
    uint64_t id = std::atomic_fetch_add<uint64_t>(&identifier_, 1);
    
    assert(id > 0);
    
    return id;
}