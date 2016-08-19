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
    enum class AnimState {
        Playing,
        Paused,
        Done
    };

    class AnimatorComponent : public Component
    {
    private:
        struct private_key {};
        friend Component;

    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        static const int32_t AnimPlayForever = -1;
        
        static std::shared_ptr<AnimatorComponent> create(ObjectId componentId);
        
        AnimatorComponent(struct private_key const& key, ObjectId componentId);
        ~AnimatorComponent() {}
        
        AnimState   state;
        int32_t     currentFrame;
        int32_t     iterations;
        float       frameRemainderTime;
        float       secPerFrame;
        float       speed;  // TODO: Add support for this
        bool        forward;
        
        // Although components should not ideally have any methods, we are doing them here to control the animation
        // TODO: Look at possibly having an animation controller to wrap around this.
        void reset();
        void setFrame(int32_t frame, bool force=false);
        void play(int32_t iterations, bool forward=true, float speed=1.0);
        void pause();
        void stop();
    };
}

#endif /* AnimatorComponent_h */
