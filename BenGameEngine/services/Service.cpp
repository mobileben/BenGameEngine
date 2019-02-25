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

void BGE::Service::outputResourceBreakdown(__attribute__ ((unused)) uint32_t numTabs) const {
}

void BGE::Service::outputMemoryBreakdown(__attribute__ ((unused)) uint32_t numTabs) const {
}

void BGE::Service::lock() const {
    pthread_mutex_lock((pthread_mutex_t *)&mutex_);
}

bool BGE::Service::trylock() const {
    return pthread_mutex_trylock((pthread_mutex_t *)&mutex_) == 0;
}

void BGE::Service::unlock() const {
    pthread_mutex_unlock((pthread_mutex_t *)&mutex_);
}
