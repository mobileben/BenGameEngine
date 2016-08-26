//
//  Event.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include <stdio.h>
#include <string>
#include <memory>
#include <functional>
#include "Handle.h"
#include "NamedObject.h"

namespace BGE {
    enum class Event {
        None,
        TouchDown,
        TouchRepeat,
        TouchDragInside,
        TouchDragOutside,
        TouchDragEnter,
        TouchDragExit,
        TouchUpInside,
        TouchUpOutside,
        TouchCancel
    };
}

template<>
struct std::hash<BGE::Event> {
    size_t operator()(const BGE::Event event) const {
        return size_t(event);
    }
};

namespace BGE {
    class GameObject;

    using EventHandlerFunction = std::function<void(SpaceHandle, GameObjectHandle, Event)>;
    
    struct EventHandler : public NamedObject {
        EventHandlerHandle          handle;
        SpaceHandle                 spaceHandle;
        GameObjectHandle            gameObjHandle;
        Event                       event;
        EventHandlerFunction        handler;
        
        EventHandler() : NamedObject() {
        }
        
        EventHandler(ObjectId objId) : NamedObject(objId) {
        }
        
        void initialze(EventHandlerHandle handle, ObjectId objId, std::string name, Event event, EventHandlerFunction function) { this->handle = handle; this->setInstanceId(objId); this->setName(name); this->event = event; this->handler = function; }
        void destroy() {
            handle = EventHandlerHandle();
            spaceHandle = SpaceHandle();
            gameObjHandle = GameObjectHandle();
            event = Event::None;
            handler = nullptr;
        }
    };
}

#endif /* Event_h */
