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
    class AnimationService : public Service
    {
    public:
        AnimationService() {}
        ~AnimationService() {}
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final;
        
        EventHandlerHandle registerEventHandler(std::string name, Event event, EventHandlerFunction function);
        void unregisterEventHandler(EventHandlerHandle handle);
        
    private:
        struct AnimationEvent {
            SpaceHandle                         spaceHandle;
            GameObjectHandle                    gameObjHandle;
            Event                               event;
        };
        
        std::unordered_map<Event, std::vector<EventHandlerHandle>>  eventHandlers_;
        std::vector<AnimationEvent>                                 events_;
        
        void queueEvent(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, Event event);
        void processEvents();

        void animateSequence(Space *space, AnimationSequenceComponent *seq, AnimatorComponent *animator, float deltaTime);
        void animateChannel(GameObject *gameObj, int32_t frame);
        void animateSequenceByFrame(Space *space, AnimationSequenceComponent *seq, FrameAnimatorComponent *animator, int32_t frame);
        int32_t handleEndOfAnim(AnimatorComponent *animator, int32_t startFrame, int32_t lastFrame);
    };
}

#endif /* AnimationService_h */
