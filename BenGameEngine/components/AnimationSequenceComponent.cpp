//
//  AnimationSequenceComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimationSequenceComponent.h"
#include "Game.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "AnimationChannelComponent.h"

uint32_t BGE::AnimationSequenceComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::AnimationSequenceComponent::type_index_ = typeid(BGE::AnimationSequenceComponent);

std::shared_ptr<BGE::AnimationSequenceComponent> BGE::AnimationSequenceComponent::create(ObjectId componentId) {
    return std::make_shared<AnimationSequenceComponent>(private_key{}, componentId);
}

BGE::AnimationSequenceComponent::AnimationSequenceComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), frameRate(0), totalFrames(0), numChannels(0), numBounds(0) {
}

BGE::AnimationSequenceComponent::~AnimationSequenceComponent() {
    // Remove all channels
    
    if (sequenceRoot) {
        
    }
}

void BGE::AnimationSequenceComponent::created() {
    auto space = getSpace();
    
    assert(space);
    
    if (space) {
        sequenceRoot = space->createObject<GameObject>();
        auto xform = space->createComponent<TransformComponent>();
        sequenceRoot->addComponent(xform);
        sequenceRoot->setActive(true);
    }
}

void BGE::AnimationSequenceComponent::setAnimationSequenceReference(AnimationSequenceReference *animSeqRef) {
    if (animSeqRef) {
        setAnimationSequenceReference(*animSeqRef);
    } else {
        NSLog(@"NULLL");
    }
}

void BGE::AnimationSequenceComponent::setAnimationSequenceReference(const AnimationSequenceReference& animSeqRef) {
    this->frameRate = animSeqRef.frameRate;
    this->totalFrames = animSeqRef.totalFrames;
    this->numChannels = animSeqRef.numChannels;
    this->numBounds = animSeqRef.numBounds;
    
    NSLog(@"setAnimationSequenceReference");
    
    sequenceRoot->setName(animSeqRef.name);
    
    // seqRoot xform children need to be purged
    auto seqXform = sequenceRoot->getComponent<TransformComponent>();
    
    if (seqXform) {
        seqXform->removeAllChildren();
    }
    
    AnimationChannelReference *channels = animSeqRef.channels;
    auto space = getSpace();

    for (auto i=0;i<numChannels;i++) {
        auto channelRef = &channels[i];
        auto obj = space->createAnimChannel(channelRef->name, channelRef);
        auto xform = obj->getComponent<TransformComponent>();

        seqXform->addChild(xform);
        
        // TODO: Destroy components when done!
        
        this->channels.push_back(obj);
        obj->setActive(true);
        
        // If this is a sequence keyframe, we need to create more game objects!
        if (channelRef->referenceType == GfxReferenceTypeKeyframe) {
            NSLog(@"Adding sequence %s/%s to channel", channelRef->name, channelRef->reference);
            auto newObj = space->createFrameAnimSequence(channelRef->reference);
            
            if (newObj) {
                auto newXform = newObj->getComponent<TransformComponent>();
                
                xform->addChild(newXform);
                
                newObj->setActive(true);
            }
        }
    }
}

void BGE::AnimationSequenceComponent::setGameObject(std::shared_ptr<GameObject> gameObject) {
    Component::setGameObject(gameObject);
    auto transformComponent = gameObject->getComponent<TransformComponent>();

    if (transformComponent) {
        // If there is a transformComponent, then add us to it, if we have
        transformComponent->addChild(sequenceRoot->getComponent<TransformComponent>());
    }
}

