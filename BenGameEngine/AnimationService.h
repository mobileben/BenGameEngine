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

namespace BGE {
    class AnimationService : public Service
    {
    public:
        AnimationService();
        ~AnimationService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime);
        
    private:
        void animateSequence(Space *space, std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<AnimatorComponent> animator, float deltaTime);
        void animateChannel(GameObject *gameObj, int32_t frame);
        void animateSequenceByFrame(Space *space, std::shared_ptr<AnimationSequenceComponent> seq, std::shared_ptr<FrameAnimatorComponent> animator, int32_t frame);
        int32_t handleEndOfAnim(std::shared_ptr<AnimatorComponent> animator, int32_t startFrame, int32_t lastFrame);
    };
}

#endif /* AnimationService_h */
