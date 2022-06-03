//
//  HeartbeatService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef HeartbeatService_h
#define HeartbeatService_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include <string>
#include <map>
#include <functional>
#include <utility>
#include <vector>
#if TARGET_OS_IPHONE
#include <QuartzCore/QuartzCore.h>
#include "BGEHeartbeatIOS.h"
#endif /* TARGET_OS_IPHONE */
#include "Queue.h"
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <thread>
#include <functional>

#include "Service.h"

namespace BGE {
    using HeartbeatDispatchFunction = std::function<void()>;
    
    class HeartbeatService : public Service {
    public:
        HeartbeatService();
        virtual ~HeartbeatService();
        
        virtual void initialize();
        virtual void reset();
        virtual void platformSuspending();
        virtual void platformResuming();
        virtual void pause();
        virtual void resume();
        virtual void destroy();
        void update(__attribute__ ((unused)) double mach_absolute_time) {}

        bool isRunning() const { return running_; }
        void setRunning(bool running);
        bool runningOnQueueThread() const;
        
        double getSecondsPerFrame() const { return secondsPerFrame_; }
        void setSecondsPerFrame(double sec) { secondsPerFrame_ = sec; }

        void dispatchAsync(HeartbeatDispatchFunction func);
        
        // For now heartbeat is done all based on one rate, however, later it will be done by interval
        void registerListener(std::string name, std::function<void(double dt)> listener, uint32_t order);
        void unregisterListener(std::string name);

#ifdef SUPPORT_PROFILING
        int64_t getProcessingTime() const { return processingTime_; }
#endif /* SUPPORT_PROFILING */

    protected:
        std::map<std::string, std::pair<std::function<void(double dt)>, uint32_t>> listeners_;
        std::vector<std::pair<std::function<void(double dt)>, uint32_t>> orderedListeners_;

#if TARGET_OS_IPHONE
        BGEHeartbeatIOS *iosHeartbeat_;
#endif /* TARGET_OS_IPHONE */
        
        bool                        running_;
        uint64_t                    counter_;
        uint64_t                    lastCounter_;
        mach_timebase_info_data_t   timebaseInfo_;
        double                      timebaseMultiplier_;
        double                      secondsPerFrame_;
        bool                        threadRunning_;
        std::mutex                  threadRunningMutex_;
        std::thread                 thread_;
        Queue<HeartbeatService *>   queuedItems_;
        
        std::mutex                              dispatchQueueMutex_;
        uint32_t                                currentDispatchQueue_;
        std::queue<HeartbeatDispatchFunction>   dispatchQueues_[2];
        
#ifdef SUPPORT_PROFILING
        int64_t     processingTime_;
#endif /* SUPPORT_PROFILING */

        void rebuildOrderedListeners();
        void queueTickHandler();
        void tickHandler();
        void dispatchHandler();
        void threadFunction();
    };
}

#endif /* HeartbeatService_h */
