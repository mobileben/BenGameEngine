//
//  FrameAnimatorComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef FrameAnimatorComponent_h
#define FrameAnimatorComponent_h

#include <stdio.h>
#include "Component.h"

namespace BGE {
    class FrameAnimatorComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        FrameAnimatorComponent() : Component(), currentFrame(0) {}
        ~FrameAnimatorComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            Component::initialize(handle, spaceHandle);
            
            currentFrame = 0;
        }
        
        void destroy() final {
            currentFrame = 0;
        
            // Component::destroy last
            Component::destroy();
        }

        // TODO: Move to controller
        int32_t     currentFrame;
    };
}

#endif /* FrameAnimatorComponent_h */
