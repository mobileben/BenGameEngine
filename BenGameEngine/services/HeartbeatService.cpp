//
//  HeartbeatService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include <cassert>
#include "HeartbeatService.h"
#include "Game.h"

#if DEBUG
#include <pthread.h>
#endif

#ifdef SUPPORT_PROFILING
#include "Profiling.h"
#endif /* SUPPORT_PROFILING */

BGE::HeartbeatService::HeartbeatService() : running_(true), counter_(0), lastCounter_(0), threadRunning_(false), currentDispatchQueue_(0) {
#if TARGET_OS_IPHONE
    iosHeartbeat_ = [[BGEHeartbeatIOS alloc] init];
#endif /* TARGET_OS_IPHONE */

    timebaseInfo_.numer = 0; timebaseInfo_.denom = 0;
    mach_timebase_info(&timebaseInfo_);
    
    timebaseMultiplier_ = (double) timebaseInfo_.numer / (double) timebaseInfo_.denom / 1000000000.0F;
    counter_ = mach_absolute_time();
    
    lastCounter_ = counter_;
}

BGE::HeartbeatService::~HeartbeatService() {
    std::lock_guard<std::mutex> lock(threadRunningMutex_);
    threadRunning_ = false;
    queuedItems_.quit();
    try {
        thread_.join();
    } catch(std::exception& e) {
        printf("Exception trying to join thread %s\n", e.what());
    }
}

void BGE::HeartbeatService::initialize() {
    thread_ = std::thread(&HeartbeatService::threadFunction, this);
    
#if TARGET_OS_IPHONE
    // Attach
    iosHeartbeat_.tickHandler = std::bind(&HeartbeatService::queueTickHandler, this);
#endif /* TARGET_OS_IPHONE */
}

void BGE::HeartbeatService::reset() {}
void BGE::HeartbeatService::platformSuspending() {
    Service::platformSuspending();

#if TARGET_OS_IPHONE
    [iosHeartbeat_ platformSuspending];
#endif /* TARGET_OS_IPHONE */
}

void BGE::HeartbeatService::platformResuming() {
    Service::platformResuming();

#if TARGET_OS_IPHONE
    [iosHeartbeat_ platformResuming];
#endif /* TARGET_OS_IPHONE */
    
    // Get our lastCounter to a good time so we don't get a huge jump in time
    lastCounter_ = mach_absolute_time();
}

void BGE::HeartbeatService::pause() {
    Service::pause();
}

void BGE::HeartbeatService::resume() {
    Service::resume();
}

void BGE::HeartbeatService::destroy() {}

void BGE::HeartbeatService::setRunning(bool running) {
    running_ = running;
}

void BGE::HeartbeatService::threadFunction() {
    pthread_setname_np("heartbeat");
    threadRunning_ = true;
    
    while (true) {
        std::unique_lock<std::mutex> lock(threadRunningMutex_);
        if (!threadRunning_) {
            lock.unlock();
            return;
        }
        lock.unlock();

        queuedItems_.pop();
        tickHandler();
    }
}

void BGE::HeartbeatService::queueTickHandler() {
    queuedItems_.pushIfEmpty(this);
}

void BGE::HeartbeatService::dispatchHandler() {
    std::unique_lock<std::mutex> lock(dispatchQueueMutex_);
    // To process queue
    auto& queue = dispatchQueues_[currentDispatchQueue_];
    
    // Flip current buffer
    currentDispatchQueue_ ^= 1;
    
    // Unlock to allow any running handler to redispatch
    lock.unlock();
    
    // Now go through each one. Since we flipped the buffer, any changes to the queue will happen
    auto count = queue.size();
    for (auto i=0u;i<count;++i) {
        auto func =  std::move(queue.front());
        queue.pop();
        func();
    }
}

void BGE::HeartbeatService::tickHandler() {
#ifdef SUPPORT_PROFILING
    auto startTime = profiling::EpochTime::timeInMicroSec();
#endif /* SUPPORT_PROFILING */
    if (!isBackgrounded() && running_) {
        counter_ = mach_absolute_time();
        
        double delta = counter_ - lastCounter_;
        double elapsedTime = delta * timebaseMultiplier_;

        lastCounter_ = counter_;
        
        // Dispatch handlers are done prior to any of the listeners
        dispatchHandler();
        
        for (auto const& entry : orderedListeners_) {
            entry.first(elapsedTime);
        }
        Game::getInstance()->getRenderService()->queueRender();
    }
#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
#endif /* SUPPORT_PROFILING */
}

void BGE::HeartbeatService::registerListener(std::string name, std::function<void(double dt)> listener, uint32_t order) {
    // Determine if we already exist, if we do, assert
    assert(listeners_.find(name) == listeners_.end());
    
    // Now make sure our orders are respected. This means orders cannot be the same
    for (auto const& entry : listeners_) {
        assert(entry.second.second != order);
    }
    
    listeners_[name] = std::make_pair(listener, order);
    
    // Now rebuild our
    rebuildOrderedListeners();
}

void BGE::HeartbeatService::unregisterListener(__attribute__ ((unused)) std::string name) {
    
}

bool BGE::HeartbeatService::runningOnQueueThread() const {
    return std::this_thread::get_id() == thread_.get_id();
}

void BGE::HeartbeatService::dispatchAsync(HeartbeatDispatchFunction func) {
    std::lock_guard<std::mutex> lock(dispatchQueueMutex_);
    auto& queue = dispatchQueues_[currentDispatchQueue_];
    queue.push(func);
}


void BGE::HeartbeatService::rebuildOrderedListeners() {
    std::vector<std::pair<std::function<void(double dt)>, uint32_t>> ordered;
    
    for (auto const& entry : listeners_) {
        uint32_t priority = entry.second.second;
        
        if (ordered.size() != 0) {
            bool added = false;
            std::vector<std::pair<std::function<void(double dt)>, uint32_t>>::iterator iter;
            
            for (iter = ordered.begin(); iter != ordered.end();iter++) {
                if (priority < iter->second) {
                    ordered.insert(iter, listeners_[entry.first]);
                    added = true;
                    break;
                }
            }
            
            if (!added) {
                ordered.push_back(listeners_[entry.first]);
            }
        } else {
            ordered.push_back(listeners_[entry.first]);
        }
    }
    
    orderedListeners_ = ordered;
}


