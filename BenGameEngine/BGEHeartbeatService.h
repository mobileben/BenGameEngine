//
//  BGEHeartbeatService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEHeartbeatService_h
#define BGEHeartbeatService_h

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

#include "BGEService.h"

class BGEHeartbeatService : public BGEService {
public:
    BGEHeartbeatService();
    virtual ~BGEHeartbeatService() {}
    
    virtual void initialize();
    virtual void reset();
    virtual void enteringBackground();
    virtual void enteringForeground();
    virtual void pause();
    virtual void resume();
    virtual void destroy();

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

#endif /* BGEHeartbeatService_h */
