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
BGE::ComponentTypeId BGE::AnimationSequenceComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimationSequenceComponent::type_index_ = typeid(BGE::AnimationSequenceComponent);

BGE::AnimationSequenceComponent::AnimationSequenceComponent() : Component(), frameRate(0), totalFrames(0), numChannels(0), numBounds(0) {
}

void BGE::AnimationSequenceComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    frameRate = 0;
    totalFrames = 0;
    numChannels = 0;
    numBounds = 0;
}

void BGE::AnimationSequenceComponent::destroy() {
    auto space = getSpace();

    for (auto const &channel : channels) {
        space->removeGameObject(channel);
    }

    channels.clear();

    bounds = nullptr;

    frameRate = 0;
    totalFrames = 0;
    numChannels = 0;
    numBounds = 0;

    // Component::destroy last
    Component::destroy();
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
    
#ifdef NOT_YET
    NSLog(@"setAnimationSequenceReference");
#endif
    auto root = getGameObject();
    auto seqXform = root->getComponent<TransformComponent>();
    
    if (seqXform) {
        seqXform->removeAllChildren();
    }
    
    AnimationChannelReference *channels = animSeqRef.channels;
    auto space = getSpace();

    for (auto i=0;i<numChannels;i++) {
        auto channelRef = &channels[i];
        auto obj = space->createAnimChannel(channelRef->reference, channelRef->name, channelRef, nullptr);
        auto xform = obj->getComponent<TransformComponent>();
        auto xformHandle = xform->getHandle<TransformComponent>();

        // Refresh root/seqXform in case handle capacity changed
        root = getGameObject();
        seqXform = root->getComponent<TransformComponent>();

        seqXform->addChild(xform);
        
        // TODO: Destroy components when done!
        
        this->channels.push_back(obj->getHandle());
        obj->setActive(true);
        
        // If this is a sequence keyframe, we need to create more game objects!
        if (channelRef->referenceType == GfxReferenceTypeKeyframe) {
#ifdef NOT_YET
            NSLog(@"Adding sequence %s/%s to channel", channelRef->name, channelRef->reference);
#endif
            auto newObj = space->createFrameAnimSequence(channelRef->reference, channelRef->name, animSeqRef.scenePackage, nullptr);
            
            if (newObj) {
                auto newXform = newObj->getComponent<TransformComponent>();
                
                // Force update xform in case a resize of handle capacity occurred
                xform = space->getComponent(xformHandle);
                
                xform->addChild(newXform);
                
                newObj->setActive(true);
            }
        }
    }
    
    bounds = animSeqRef.bounds;
}

