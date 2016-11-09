//
//  AnimationChannelComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimationChannelComponent.h"
#include "Game.h"
#include "GameObject.h"
#include "SpriteRenderComponent.h"
#include "BoundingBoxComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "MaskComponent.h"
#include "PlacementComponent.h"

uint32_t BGE::AnimationChannelComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::AnimationChannelComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimationChannelComponent::type_index_ = typeid(BGE::AnimationChannelComponent);
