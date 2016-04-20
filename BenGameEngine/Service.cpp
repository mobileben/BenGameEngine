//
//  Service.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Service.h"
#include <cassert>

uint64_t BGE::Service::getIdAndIncrement() {
    uint64_t id = std::atomic_fetch_add<uint64_t>(&identifier_, 1);
    
    assert(id > 0);
    
    return id;
}
