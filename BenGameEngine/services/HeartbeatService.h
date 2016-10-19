//
//  HeartbeatService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef HeartbeatService_h
#define HeartbeatService_h

#include <stdio.h>
#include <string>
#include <map>
#include <functional>
#include <utility>
#include <vector>
#include <QuartzCore/QuartzCore.h>
#include "BGEHeartbeatIOS.h"
#include <mach/mach.h>
#include <mach/mach_time.h>

#include "Service.h"

namespace BGE {
    class HeartbeatService : public Service {
    public:
        HeartbeatService();
        virtual ~HeartbeatService() {}
        
        virtual void initialize();
        virtual void reset();
        virtual void platformSuspending();
        virtual void platformResuming();
        virtual void pause();
        virtual void resume();
        virtual void destroy();
        void update(double deltaTime) {}

        bool isRunning() const { return running_; }
        void setRunning(bool running);
        
        // For now heartbeat is done all based on one rate, however, later it will be done by interval
        void registerListener(std::string name, std::function<void(double dt)> listener, uint32_t order);
        void unregisterListener(std::string name);
        
    protected:
        std::map<std::string, std::pair<std::function<void(double dt)>, uint32_t>> listeners_;
        std::vector<std::pair<std::function<void(double dt)>, uint32_t>> orderedListeners_;
        
        BGEHeartbeatIOS *iosHeartbeat_;
        
        bool running_;
        uint64_t counter_;
        uint64_t lastCounter_;
        mach_timebase_info_data_t timebaseInfo_;
        double timebaseMultiplier_;
        
        void rebuildOrderedListeners();
        void tickHandler();
    };
}

#endif /* HeartbeatService_h */
