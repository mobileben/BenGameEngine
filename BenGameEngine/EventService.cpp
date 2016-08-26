//
//  EventService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "EventService.h"

BGE::EventService::EventService() : handleService_(InitialEventHandlerReserve, HandleServiceNoMaxLimit) {
}

BGE::EventHandlerHandle BGE::EventService::createEventHandlerHandle(std::string name, Event event, EventHandlerFunction function) {
    EventHandlerHandle handle;
    auto eventHandler = handleService_.allocate(handle);
 
    if (eventHandler) {
        ObjectId eventHandlerId = getIdAndIncrement();

        eventHandler->initialze(handle, eventHandlerId, name, event, function);
    }
    
    return handle;
}

BGE::EventHandler *BGE::EventService::getEventHandler(EventHandlerHandle handle) {
    return handleService_.dereference(handle);
}

void BGE::EventService::removeEventHandler(EventHandlerHandle handle) {
    auto eventHandler = getEventHandler(handle);
    
    if (eventHandler) {
        eventHandler->destroy();
    }
    
    handleService_.release(handle);
}
