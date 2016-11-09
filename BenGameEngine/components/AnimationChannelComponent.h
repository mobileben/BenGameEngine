//
//  AnimationChannelComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimationChannelComponent_h
#define AnimationChannelComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Component.h"
#include "GraphicFormats.h"
#include "Handle.h"

namespace BGE {
    class AnimationChannelComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        const AnimationChannelReference *channel;

        AnimationChannelComponent() : Component(), channel(nullptr) {}        
        ~AnimationChannelComponent() {}
        
        inline void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            Component::initialize(handle, spaceHandle);
            
            channel = nullptr;
        }
        
        inline void destroy() final {
            channel = nullptr;
            
            // Component::destroy last
            Component::destroy();
        }

    protected:
        friend ComponentService;
        friend GameObject;
        friend Space; // Do this since channel may get modified during handle expansion
    };
}

#endif /* AnimationChannelComponent_h */
