//
//  BGEHeartbeatService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/27/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include <cassert>
#include "BGEHeartbeatService.h"

BGEHeartbeatService::BGEHeartbeatService() : running_(true), counter_(0), lastCounter_(0) {
    iosHeartbeat_ = [[BGEHeartbeatIOS alloc] init];
    
    timebaseInfo_.numer = 0; timebaseInfo_.denom = 0;
    mach_timebase_info(&timebaseInfo_);
    
    timebaseMultiplier_ = (double) timebaseInfo_.numer / (double) timebaseInfo_.denom / 1000000000.0F;
    counter_ = mach_absolute_time();
    
    lastCounter_ = counter_;
    
    // Attach
    iosHeartbeat_.tickHandler = std::bind(&BGEHeartbeatService::tickHandler, this);
}

void BGEHeartbeatService::initialize() {}
void BGEHeartbeatService::reset() {}
void BGEHeartbeatService::enteringBackground() {}
void BGEHeartbeatService::enteringForeground() {}
void BGEHeartbeatService::pause() {}
void BGEHeartbeatService::resume() {}
void BGEHeartbeatService::destroy() {}

void BGEHeartbeatService::setRunning(bool running) {
    running_ = running;
}

void BGEHeartbeatService::tickHandler() {
    if (running_) {
        counter_ = mach_absolute_time();
        
        double delta = counter_ - lastCounter_;
        double elapsedTime = delta * timebaseMultiplier_;
        
        lastCounter_ = counter_;
        
        NSLog(@"HERE WITH TICK %f", elapsedTime);
        
        for (auto const& entry : orderedListeners_) {
            entry.first(elapsedTime);
        }
//        tick(elapsedTime);

    }
}

void BGEHeartbeatService::registerListener(std::string name, std::function<void(double dt)> listener, uint32_t order) {
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

void BGEHeartbeatService::unregisterListener(std::string name) {
    
}

void BGEHeartbeatService::rebuildOrderedListeners() {
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


