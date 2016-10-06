//
//  Service.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Service.h"
#include <cassert>

BGE::Service::Service() : identifier_(1), backgrounded_(false), paused_(false) {
    // Initialize our mutex
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex_, &attr);
}

BGE::ObjectId BGE::Service::getIdAndIncrement() {
    ObjectId id = std::atomic_fetch_add<ObjectId>(&identifier_, 1);
    
    assert(id > 0);
    
    return id;
}

void BGE::Service::lock() {
    pthread_mutex_lock(&mutex_);
}

void BGE::Service::unlock() {
    pthread_mutex_unlock(&mutex_);
}

