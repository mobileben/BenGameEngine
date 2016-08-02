//
//  EventService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef EventService_h
#define EventService_h

#include <stdio.h>
#include "Service.h"
#include "Event.h"
#include "HandleService.h"

namespace BGE {
    class EventService : public Service {
    public:
        EventService();
        ~EventService() {}
        
        EventHandlerHandle createEventHandlerHandle(std::string name, Event event, EventHandlerFunction function);
        
        EventHandler *getEventHandler(EventHandlerHandle handle);
        
        void removeEventHandler(EventHandlerHandle handle);
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}
        
    protected:
        
    private:
        static const uint32_t InitialEventHandlerReserve = 128;
        
        using EventHandlerHandleService = HandleService<EventHandler, EventHandlerHandle>;
        
        EventHandlerHandleService handleService_;
    };
}

#endif /* EventService_h */
