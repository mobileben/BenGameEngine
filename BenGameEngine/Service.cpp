//
//  Service.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Service.h"
#include <cassert>

BGE::ObjectId BGE::Service::getIdAndIncrement() {
    ObjectId id = std::atomic_fetch_add<ObjectId>(&identifier_, 1);
    
    assert(id > 0);
    
    return id;
}
