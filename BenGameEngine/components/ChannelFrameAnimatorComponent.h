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
    private:
        struct private_key {};
        friend Component;

    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;

        static std::shared_ptr<ChannelFrameAnimatorComponent> create(ObjectId componentId);
        
        ChannelFrameAnimatorComponent(ObjectId objId) : Component(objId), currKeyframe(0) {}
        ChannelFrameAnimatorComponent(struct private_key const& key, ObjectId componentId);
        ~ChannelFrameAnimatorComponent() {}
        
        // TODO: Move to controller
        int32_t     currKeyframe;
    };
}

#endif /* ChannelFrameAnimatorComponent_h */
