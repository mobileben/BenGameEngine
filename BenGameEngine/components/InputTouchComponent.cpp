//
//  InputTouchComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "InputTouchComponent.h"

uint32_t BGE::InputTouchComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::InputTouchComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::InputTouchComponent::type_index_ = typeid(BGE::InputTouchComponent);

void BGE::InputTouchComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);

#if TARGET_OS_IPHONE
    touch = nil;
#endif /* TARGET_OS_IPHONE */
}

