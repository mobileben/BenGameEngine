//
//  InputService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputService_h
#define InputService_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include <functional>
#include "Service.h"
#include "Input.h"
#include "Event.h"
#include "HandleService.h"
#include "MathTypes.h"

#include <mutex>

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
    class ButtonComponent;
    class EventService;
    class GameObject;
    class Space;
    
    class InputService : public Service {
    public:
        InputService(std::shared_ptr<EventService>);
        ~InputService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final;
        void garbageCollect() final;

#if TARGET_OS_IPHONE
        void touchEventDown(NSSet* touches, UIView* view);
        void touchEventUp(NSSet* touches, UIView* view);
        void touchEventMove(NSSet* touches, UIView* view);
        void touchEventCancel(NSSet* touches, UIView* view);
#endif /* TARGET_OS_IPHONE */
        
        EventHandlerHandle registerEventHandler(GameObject *gameObj, Event event, EventHandlerFunction function);
        void unregisterEventHandler(EventHandlerHandle handle);
        void spaceReset(Space *space);

        bool getBboxPoints(std::vector<Vector3>& bbox, std::vector<Vector3>& scaledBbox);

#ifdef SUPPORT_PROFILING
        int32_t getNumProcessedObjects() const { return numProcessedObjects_; }
        int64_t getProcessingTime() const { return processingTime_; }
#endif /* SUPPORT_PROFILING */

    private:
        InputService() = delete;
        
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
        
        struct InputEventItem {
            Space                               *space;
            GameObjectHandle                    gameObjHandle;
            ButtonComponentHandle               buttonComponentHandle;
            InputTouchComponentHandle           inputTouchComponentHandle;
            TouchType                           touchType;
            Event                               event;
            bool                                inBounds;
        };
        
        static const uint32_t InitialInputReserve = 64;
        
        using InputHandleService = HandleService<Input, InputHandle>;
        
        using InputVector = std::vector<Input *>;
        
        InputHandleService                                                                  handleService_;
        std::shared_ptr<EventService>                                                       eventService_;
        InputVector                                                                         inputs_;
        std::vector<InputEventItem>                                                         inputEventQueue_;
        std::vector<InputEventItem>                                                         inputEventWorkQueue_;
        std::vector<InputButtonHandler>                                                     inputButtonHandlers_;
        std::unordered_map<Event, std::vector<EventHandlerHandle>>                          inputEventHandlers_;

        std::mutex                                                                          bboxMutex_;
        std::vector<Vector3>                                                                bboxPoints_;
        std::vector<Vector3>                                                                scaledBBoxPoints_;

        std::vector<SpaceHandle>                                                            spaceHandles_;
        std::vector<Space *>                                                                spaces_;
        std::vector<std::vector<GameObject *>>                                              rootGameObjects_;
        
#ifdef SUPPORT_PROFILING
        int32_t     numProcessedObjects_;
        int64_t     processingTime_;
#endif /* SUPPORT_PROFILING  */
        
        Input *createInput();
#if TARGET_OS_IPHONE
        void touchEvent(TouchType type, NSSet* touches, UIView* view);
#endif /* TARGET_OS_IPHONE */
        void checkInput(Input *input, Space *space, GameObject *gameObj, std::vector<InputEventItem> &queue);
        void updateInputObject(Space *space, GameObject *gameObj, double deltaTime);
        void getInputPoints(Space *space, GameObject *gameObj, std::vector<Vector3>& bboxPoints, std::vector<Vector3>& scaledBBoxPoints);
    };
}

#endif /* InputService_h */
