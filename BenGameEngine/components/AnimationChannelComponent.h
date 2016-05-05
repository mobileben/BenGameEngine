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

namespace BGE {
    class AnimationChannelComponent : public Component {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<AnimationChannelComponent> create(uint64_t componentId);
        static std::shared_ptr<AnimationChannelComponent> create(uint64_t componentId, std::string name);
        
        AnimationChannelComponent(struct private_key const& key, uint64_t componentId);
        AnimationChannelComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        virtual ~AnimationChannelComponent() {}
        
    protected:
        friend BGE::ComponentService;
        
        AnimationChannelComponent(uint64_t componentId);
        AnimationChannelComponent(uint64_t componentId, std::string name);
        
    private:
        uint32_t    keyframe_;
        uint32_t    totalKeyframes_;
        uint32_t    iterations_;
        float       frameRemainderTime_;

        std::vector<std::shared_ptr<AnimationChannelComponent>> childrenChannels_;
    };
}

#endif /* AnimationChannelComponent_h */
