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

namespace BGE {
    class AnimationChannelComponent : public Component {
    private:
        struct private_key {};
        
    public:
        const AnimationChannelReference *channel;

        static std::shared_ptr<AnimationChannelComponent> create(uint64_t componentId);
        static std::shared_ptr<AnimationChannelComponent> create(uint64_t componentId, std::string name);
        
        AnimationChannelComponent(struct private_key const& key, uint64_t componentId);
        AnimationChannelComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
        virtual ~AnimationChannelComponent() {}
        
        void setAnimationChannelReference(const AnimationChannelReference *animChanRef);
        
        void updateReference();

    protected:
        friend ComponentService;
        friend GameObject;
        
        AnimationChannelComponent(uint64_t componentId);
        AnimationChannelComponent(uint64_t componentId, std::string name);
        
        void setGameObject(std::shared_ptr<GameObject> gameObject);
    };
}

#endif /* AnimationChannelComponent_h */
