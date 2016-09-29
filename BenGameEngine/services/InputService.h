//
//  InputService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputService_h
#define InputService_h

#include <stdio.h>
#include <functional>
#include "Service.h"
#include "Input.h"
#include "Event.h"
#include "HandleService.h"

namespace BGE {
    enum class InputTouchEvent {
        Down,
        Repeat,
        UpInside,
        UpOutside,
        Cancel,
        ToggleValueChanged
    };
    
    using InputTouchHandlerKey = std::pair<std::string, InputTouchEvent>;
}

template<>
struct std::hash<BGE::InputTouchEvent> {
    size_t operator()(const BGE::InputTouchEvent event) const {
        return size_t(event);
    }
};

template <>
struct std::hash<BGE::InputTouchHandlerKey> {
    size_t operator()(const BGE::InputTouchHandlerKey key) const {
        size_t prime = 31;
        size_t result = 1;
        
        result = prime * result + hash<string>()(key.first);
        result = prime * result + hash<BGE::InputTouchEvent>()(key.second);

        return result;
    }
};

namespace BGE {
    class GameObject;
    class ButtonComponent;
    
    class InputService : public Service {
    public:
        InputService();
        ~InputService() {}
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}
        
        void touchEventDown(NSSet* touches, UIView* view);
        void touchEventUp(NSSet* touches, UIView* view);
        void touchEventMove(NSSet* touches, UIView* view);
        void touchEventCancel(NSSet* touches, UIView* view);

        void process();
        
        EventHandlerHandle registerEventHandler(std::string name, Event event, EventHandlerFunction function);
        void unregisterEventHandler(EventHandlerHandle handle);
        
    private:
        struct InputButtonHandler {
            SpaceHandle                         spaceHandle;
            GameObjectHandle                    gameObjHandle;
            ButtonComponentHandle               buttonComponentHandle;
            TouchType                           touchType;
            bool                                inBounds;
        };

        struct InputTouchEvent {
            SpaceHandle                         spaceHandle;
            GameObjectHandle                    gameObjHandle;
        };
        
        static const uint32_t InitialInputReserve = 64;
        
        using InputHandleService = HandleService<Input, InputHandle>;
        
        using InputVector = std::vector<Input *>;
        
        InputHandleService                                                                  handleService_;
        InputVector                                                                         inputs_;
        std::vector<InputButtonHandler>                                                     inputButtonHandlers_;
        std::unordered_map<Event, std::vector<EventHandlerHandle>>                          inputEventHandlers_;
        
        Input *createInput();
        void touchEvent(TouchType type, NSSet* touches, UIView* view);
    };
}

#endif /* InputService_h */