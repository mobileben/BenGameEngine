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
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<FrameAnimatorComponent> create(uint64_t componentId);
        static std::shared_ptr<FrameAnimatorComponent> create(uint64_t componentId, std::string name);
        
        FrameAnimatorComponent(struct private_key const& key, uint64_t componentId);
        FrameAnimatorComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        virtual ~FrameAnimatorComponent() {}
        
        // TODO: Move to controller
        int32_t     currentFrame;
    };
}

#endif /* FrameAnimatorComponent_h */
