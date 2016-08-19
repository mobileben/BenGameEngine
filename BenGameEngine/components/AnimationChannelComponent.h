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
    private:
        struct private_key {};
        friend Component;

    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        const AnimationChannelReference *channel;

        static std::shared_ptr<AnimationChannelComponent> create(ObjectId componentId);
        
        AnimationChannelComponent(struct private_key const& key, ObjectId componentId);
        AnimationChannelComponent() : Component(), channel(nullptr) {
        }
        
        ~AnimationChannelComponent() {}
        
        void setAnimationChannelReference(const AnimationChannelReference *animChanRef);
        
        void updateReference();

    protected:
        friend ComponentService;
        friend GameObject;
        
        void setGameObjectHandle(GameObjectHandle handle);
    };
}

#endif /* AnimationChannelComponent_h */
