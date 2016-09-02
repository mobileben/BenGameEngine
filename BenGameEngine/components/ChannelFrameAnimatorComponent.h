//
//  ChannelFrameAnimatorComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ChannelFrameAnimatorComponent_h
#define ChannelFrameAnimatorComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {
    class ChannelFrameAnimatorComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;

        ChannelFrameAnimatorComponent() : Component(), currKeyframe(0) {}
        ~ChannelFrameAnimatorComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            Component::initialize(handle, spaceHandle);
            
            currKeyframe = 0;
        }
        
        void destroy() final {
            currKeyframe = 0;
            
            // Component::destroy last
            Component::destroy();
        }

        // TODO: Move to controller
        int32_t     currKeyframe;
    };
}

#endif /* ChannelFrameAnimatorComponent_h */
