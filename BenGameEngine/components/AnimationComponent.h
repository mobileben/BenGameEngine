//
//  AnimationComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimationComponent_h
#define AnimationComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Component.h"

namespace BGE {
    enum class AnimState {
        Off,
        Playing,
        Stopping,
        Done
    };

    class AnimationChannelComponent;
    
    class AnimationComponent : public Component
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<AnimationComponent> create(uint64_t componentId);
        static std::shared_ptr<AnimationComponent> create(uint64_t componentId, std::string name);
        
        AnimationComponent(struct private_key const& key, uint64_t componentId);
        AnimationComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        virtual ~AnimationComponent() {}
        
    private:
        AnimState   state_;
        uint32_t    keyframe_;
        uint32_t    iterations_;
        float       frameRemainderTime_;
        
        // AnimationChannel
        //  GameObject
        //  Pointer to meta
        std::vector<std::shared_ptr<AnimationChannelComponent>> childrenChannels_;
    };
}

#endif /* AnimationComponent_h */
