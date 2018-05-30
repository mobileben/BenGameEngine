//
//  AnimatorComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimatorComponent_h
#define AnimatorComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Space.h"
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {
    class AnimationSequenceComponent;
    class FrameAnimatorComponent;

    enum class AnimState {
        Playing,
        Paused,
        Done
    };

    const int32_t AnimPlayOnce = 1;
    const int32_t AnimPlayForever = -1;
    
    class AnimatorComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        AnimatorComponent();
        ~AnimatorComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        
        AnimState   state;
        int32_t     startFrame;
        int32_t     endFrame;
        int32_t     currentFrame;
        int32_t     iterations;
        float       frameRemainderTime;
        float       secPerFrame;
        float       speed;  // TODO: Add support for this
        bool        forward;
        
        // Although components should not ideally have any methods, we are doing them here to control the animation
        // TODO: Look at possibly having an animation controller to wrap around this.
        void reset();
        void setFrame(int32_t frame);
        void setToLastFrame();
        void play(int32_t iterations=1, bool forward=true, float speed=1.0);
        void playToFrame(int32_t endFrame, float speed=1.0);
        
        void reverse(float speed=1.0); // Reverse goes the opposite direction, preserving the currentFrame
        
        void pause();
        void resume(float speed=1.0);  // Resumes from pause
        
        void stop();
        
    private:
        void animateChannel(GameObject *gameObj, int32_t frame);
        void animateSequenceByFrame(Space *space, AnimationSequenceComponent *seq, FrameAnimatorComponent *animator, int32_t frame) ;
    };
}

#endif /* AnimatorComponent_h */
