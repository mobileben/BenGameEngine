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
uint32_t BGE::AnimationSequenceComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimationSequenceComponent::type_index_ = typeid(BGE::AnimationSequenceComponent);

BGE::AnimationSequenceComponent::AnimationSequenceComponent() : Component(), frameRate(0), totalFrames(0), numChannels(0), numBounds(0) {
}

BGE::AnimationSequenceComponent::~AnimationSequenceComponent() {
    // Remove all channels
    
    auto space = getSpace();
    
    if (space) {
        space->removeGameObject(sequenceRootHandle);
    }
}

void BGE::AnimationSequenceComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    auto space = getSpace();
    
    assert(space);
    
    if (space) {
        auto sequenceRoot = space->createGameObject();
        sequenceRootHandle = sequenceRoot->getHandle();
        
        auto xform = space->createComponent<TransformComponent>();
        sequenceRoot->addComponent(xform);
        sequenceRoot->setActive(true);
    }
}

void BGE::AnimationSequenceComponent::setAnimationSequenceReference(AnimationSequenceReference *animSeqRef) {
    if (animSeqRef) {
        setAnimationSequenceReference(*animSeqRef);
    }
}

void BGE::AnimationSequenceComponent::setAnimationSequenceReference(const AnimationSequenceReference& animSeqRef) {
    this->frameRate = animSeqRef.frameRate;
    this->totalFrames = animSeqRef.totalFrames;
    this->numChannels = animSeqRef.numChannels;
    this->numBounds = animSeqRef.numBounds;
    
    NSLog(@"setAnimationSequenceReference");
    
    auto sequenceRoot = getSpace()->getGameObject(sequenceRootHandle);
    
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
        
        this->channels.push_back(obj->getHandle());
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
    
    bounds = animSeqRef.bounds;
}

void BGE::AnimationSequenceComponent::setGameObjectHandle(GameObjectHandle handle) {
    Component::setGameObjectHandle(handle);
    
    auto space = getSpace();
    auto gameObject = space->getGameObject(handle);
    auto transformComponent = gameObject->getComponent<TransformComponent>();

    if (transformComponent) {
        auto sequenceRoot = getSpace()->getGameObject(sequenceRootHandle);
        // If there is a transformComponent, then add us to it, if we have
        transformComponent->addChild(sequenceRoot->getComponent<TransformComponent>());
    }
}

