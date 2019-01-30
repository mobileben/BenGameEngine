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
    class GameObject;
    class Space;
    
    class EventService : public Service {
    public:
        EventService();
        ~EventService() {}
        
        EventHandlerHandle createEventHandlerHandle(GameObject *gameObj, Event event, EventHandlerFunction function);
        
        EventHandler *getEventHandler(EventHandlerHandle handle) const;
        
        void removeEventHandler(EventHandlerHandle handle);
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(__attribute__ ((unused)) double deltaTime) final {}
        void garbageCollect() final { handleService_.garbageCollect(); }
        
        void spaceReset(Space *space);
        
    protected:
        
    private:
        static const uint32_t InitialEventHandlerReserve = 128;
        
        using EventHandlerHandleService = HandleService<EventHandler, EventHandlerHandle>;
        
        EventHandlerHandleService handleService_;
    };
}

#endif /* EventService_h */
