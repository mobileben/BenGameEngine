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
        
        EventHandler *getEventHandler(EventHandlerHandle handle) const;
        
        void removeEventHandler(EventHandlerHandle handle);
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}
        
    protected:
        
    private:
        static const uint32_t InitialEventHandlerReserve = 128;
        
        using EventHandlerHandleService = HandleService<EventHandler, EventHandlerHandle>;
        
        EventHandlerHandleService handleService_;
    };
}

#endif /* EventService_h */
