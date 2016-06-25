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
        
    public:
        static std::shared_ptr<AnimationSequenceComponent> create(uint64_t componentId);
        static std::shared_ptr<AnimationSequenceComponent> create(uint64_t componentId, std::string name);
        
        AnimationSequenceComponent(struct private_key const& key, uint64_t componentId);
        AnimationSequenceComponent(struct private_key const& key, uint64_t componentId, std::string name);
        
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
