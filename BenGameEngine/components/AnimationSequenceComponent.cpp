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

