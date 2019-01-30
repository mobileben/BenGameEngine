//
//  EventService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "EventService.h"
#include "GameObject.h"

BGE::EventService::EventService() : handleService_(InitialEventHandlerReserve, HandleServiceNoMaxLimit) {
}

BGE::EventHandlerHandle BGE::EventService::createEventHandlerHandle(GameObject *gameObj, Event event, EventHandlerFunction function) {
    EventHandlerHandle handle;
    auto eventHandler = handleService_.allocate(handle);
 
    if (eventHandler) {
        auto space = gameObj->getSpace();
        eventHandler->initialize(handle, space->getHandle(), gameObj->getHandle(), event, function);
    }
    
    return handle;
}

BGE::EventHandler *BGE::EventService::getEventHandler(EventHandlerHandle handle) const {
    return handleService_.dereference(handle);
}

void BGE::EventService::removeEventHandler(EventHandlerHandle handle) {
    auto eventHandler = getEventHandler(handle);
    
    if (eventHandler) {
        eventHandler->destroy();
    }
    
    handleService_.release(handle);
}

void BGE::EventService::spaceReset(__attribute__ ((unused)) Space *space) {
    
}
