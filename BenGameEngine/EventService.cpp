//
//  EventService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "EventService.h"

BGE::EventService::EventService() : handleService_(InitialEventHandlerReserve, EventHandlerHandleService::NoMaxLimit) {
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
    handleService_.release(handle);
}
