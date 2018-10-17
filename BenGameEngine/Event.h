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
        TouchDownInside,
        TouchRepeatInside,
        TouchUpInside,
        TouchInsideForDuration,
        TouchUpOutside,
        TouchCancel,
        
        ValueChanged,
        
        AnimationReachedEnd,
        AnimationReachedBeginning
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

    using EventHandlerFunction = std::function<void(GameObject *, Event)>;
#if TARGET_OS_IPHONE
    
    typedef void (*ObjCGameObjectEventHandlerFunction)(__weak id, SEL, GameObject *, Event);
    
#endif /* TARGET_OS_IPHONE */

    struct EventHandler : public Object {
        EventHandlerHandle          handle;
        SpaceHandle                 spaceHandle;
        GameObjectHandle            gameObjHandle;
        Event                       event;
        EventHandlerFunction        handler;
        
        EventHandler() : Object() {
        }
        
        EventHandler(ObjectId objId) : Object(objId) {
        }
        
        void initialize(EventHandlerHandle handle, SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, Event event, EventHandlerFunction function) { this->handle = handle; this->spaceHandle = spaceHandle; this->gameObjHandle = gameObjHandle; this->event = event; this->handler = function; }
        void destroy() {
            handle = EventHandlerHandle();
            spaceHandle  = SpaceHandle();
            gameObjHandle = GameObjectHandle();
            event = Event::None;
            handler = nullptr;
        }
    };
}

#endif /* Event_h */
