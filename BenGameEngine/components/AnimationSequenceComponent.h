//
//  AnimationSequenceComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimationSequenceComponent_h
#define AnimationSequenceComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {

    class AnimationChannelComponent;
    
    class AnimationSequenceComponent : public Component
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;

    public:
        static std::shared_ptr<AnimationSequenceComponent> create(ObjectId componentId);
        
        AnimationSequenceComponent(struct private_key const& key, ObjectId componentId);
        ~AnimationSequenceComponent();
        
        void setAnimationSequenceReference(AnimationSequenceReference& animSeqRef);

        // TODO: Handle to AnimationSequenceReference
        uint32_t    frameRate;
        uint32_t    totalFrames;
        uint32_t    numChannels;
        uint32_t    numBounds;
        
        // AnimationChannel
        //  GameObject
        std::shared_ptr<GameObject> sequenceRoot;
        
        //  Pointer to meta
        std::vector<std::shared_ptr<GameObject>> channels;
        
    protected:
        friend GameObject;
        friend ComponentService;
        
        void setGameObject(std::shared_ptr<GameObject> gameObject);
        void created();
    };
}

#endif /* AnimationComponent_h */
