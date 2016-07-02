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
    }
}

void BGE::AnimationSequenceComponent::setAnimationSequenceReference(AnimationSequenceReference& animSeqRef) {
    this->frameRate = animSeqRef.frameRate;
    this->totalFrames = animSeqRef.totalFrames;
    this->numChannels = animSeqRef.numChannels;
    this->numBounds = animSeqRef.numBounds;
    
    NSLog(@"setAnimationSequenceReference");
    
    // seqRoot xform children need to be purged
    auto seqXform = sequenceRoot->getComponent<TransformComponent>();
    
    if (seqXform) {
        seqXform->removeAllChildren();
    }
    
    AnimationChannelReference *channels = animSeqRef.channels;
    auto space = getSpace();

    for (auto i=0;i<numChannels;i++) {
        auto obj = space->createObject<GameObject>();
        auto xform = space->createComponent<TransformComponent>();
        auto channel = space->createComponent<AnimationChannelComponent>();
        auto animator = space->createComponent<ChannelFrameAnimatorComponent>();
        auto channelRef = &channels[i];
        
        channel->setAnimationChannelReference(channelRef);
        
        NSLog(@"Adding channel %s/%s/%d", channelRef->name, channelRef->reference, channelRef->referenceType);
        
        // Channel game objects will have: xform, channel, channel frame animator, render (optional)
        obj->addComponent(xform);
        obj->addComponent(channel);
        obj->addComponent(animator);
        
        seqXform->addChild(xform);
        
        // TODO: Destroy components when done!
        
        this->channels.push_back(obj);
        
        // If this is a sequence keyframe, we need to create more game objects!
        if (channelRef->referenceType == GfxReferenceTypeKeyframe) {
            auto seqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(channelRef->reference);
            
            NSLog(@"Adding sequence %s/%s to channel", channelRef->name, channelRef->reference);

            if (seqRef) {
                auto newSeq = space->createComponent<AnimationSequenceComponent>();
                // Create new game object and add to our xform
                auto newObj = space->createObject<GameObject>(seqRef->name);
                
                if (newObj) {
                    auto newXform = space->createComponent<TransformComponent>();
                    auto newAnimator = space->createComponent<FrameAnimatorComponent>();
                    
                    newObj->addComponent(newXform);
                    newObj->addComponent(newSeq);
                    newObj->addComponent(newAnimator);
                    
                    xform->addChild(newXform);
                }
                
                // Once we've added all components, then add the seq ref
                newSeq->setAnimationSequenceReference(*seqRef);
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
    
    for (auto i=0;i<numChannels;i++) {
        auto gameObj = channels[i];
    }
}

