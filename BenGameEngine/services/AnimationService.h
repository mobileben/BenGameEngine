//
//  AnimationService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimationService_h
#define AnimationService_h

#include <stdio.h>
#include "Service.h"
#include "GameObject.h"
#include "AnimationSequenceComponent.h"
#include "AnimationChannelComponent.h"
#include "AnimatorComponent.h"
#include "ChannelFrameAnimatorComponent.h"
#include "FrameAnimatorComponent.h"
#include "Event.h"

namespace BGE {
    class EventService;
    class Space;
    
    class AnimationService : public Service
    {
    public:
        AnimationService(std::shared_ptr<EventService> eventService) : eventService_(eventService) {}
        ~AnimationService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final;
        void garbageCollect() final;
        
        EventHandlerHandle registerEventHandler(GameObject *gameObj, Event event, EventHandlerFunction function);
        void unregisterEventHandler(EventHandlerHandle handle);
        void spaceReset(Space *space);

#ifdef SUPPORT_PROFILING
        int32_t getNumProcessedObjects() const { return numProcessedObjects_;}
        int64_t getProcessingTime() const { return processingTime_; }
#endif /* SUPPORT_PROFILING */

    private:
        AnimationService() = delete;
        
        struct AnimationEvent {
            SpaceHandle                         spaceHandle;
            GameObjectHandle                    gameObjHandle;
            Event                               event;
        };
        
        std::shared_ptr<EventService>                               eventService_;
        std::unordered_map<Event, std::vector<EventHandlerHandle>>  eventHandlers_;
        std::vector<AnimationEvent>                                 events_;

        std::vector<SpaceHandle>                                    spaceHandles_;

#ifdef SUPPORT_PROFILING
        int32_t     numProcessedObjects_;
        int64_t     processingTime_;
#endif /* SUPPORT_PROFILING */
        
        void queueEvent(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, Event event);
        void processEvents();

        void animateSequence(Space *space, AnimationSequenceComponent *seq, AnimatorComponent *animator, float deltaTime);
        void animateChannel(GameObject *gameObj, int32_t frame);
        void animateSequenceByFrame(Space *space, AnimationSequenceComponent *seq, FrameAnimatorComponent *animator, int32_t frame);
        int32_t handleEndOfAnim(AnimatorComponent *animator, int32_t startFrame, int32_t lastFrame);
    };
}

#endif /* AnimationService_h */
