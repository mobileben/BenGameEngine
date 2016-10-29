//
//  HandleService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 10/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "HandleService.h"
#include <pthread.h>

static pthread_mutex_t handleServiceMutex_;

void BGE::handleServicesInitialize() {
    // Initialize our mutex
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&handleServiceMutex_, &attr);
}

void BGE::handleServicesLock() {
    pthread_mutex_lock(&handleServiceMutex_);
}

void BGE::handleServicesUnlock() {
    pthread_mutex_unlock(&handleServiceMutex_);
}
