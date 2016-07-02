//
//  FrameAnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FrameAnimatorComponent.h"

std::shared_ptr<BGE::FrameAnimatorComponent> BGE::FrameAnimatorComponent::create(ObjectId componentId) {
    return std::make_shared<FrameAnimatorComponent>(private_key{}, componentId);
}

BGE::FrameAnimatorComponent::FrameAnimatorComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), currentFrame(0) {
}
