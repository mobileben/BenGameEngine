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
        
    public:
        static std::shared_ptr<ChannelFrameAnimatorComponent> create(uint64_t componentId);
        static std::shared_ptr<ChannelFrameAnimatorComponent> create(uint64_t componentId, std::string name);
        
        ChannelFrameAnimatorComponent(struct private_key const& key, uint64_t componentId);
        ChannelFrameAnimatorComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        virtual ~ChannelFrameAnimatorComponent() {}
        
        // TODO: Move to controller
        int32_t     currKeyframe;
    };
}

#endif /* ChannelFrameAnimatorComponent_h */
