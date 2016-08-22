//
//  ComponentService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ComponentService.h"
#include "AnimationChannelComponent.h"
#include "AnimationSequenceComponent.h"
#include "AnimatorComponent.h"
#include "ChannelFrameAnimatorComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "FlatRectRenderComponent.h"
#include "FrameAnimatorComponent.h"
#include "LineRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "ButtonComponent.h"
#include "MaskComponent.h"
#include "TextureMaskComponent.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"

bool BGE::ComponentService::componentsRegistered_ = false;
std::vector<void *> BGE::ComponentService::componentHandleServices_;

void BGE::ComponentService::registerComponents() {
    if (!ComponentService::componentsRegistered_) {
        registerComponent<TransformComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<BoundingBoxComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimationChannelComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimationSequenceComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimatorComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<ChannelFrameAnimatorComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<ColorMatrixComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<ColorTransformComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<FrameAnimatorComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<SpriteRenderComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<TextComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<ButtonComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<InputTouchComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<LineRenderComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<FlatRectRenderComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<MaskComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<TextureMaskComponent>(1024, HandleServiceNoMaxLimit);
    }
    
    componentsRegistered_ = true;
}

BGE::ComponentService::ComponentService() : Service() {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
    }
    
    // Create our componentHandle vectors
    for (auto it=componentHandleServices_.begin();it!=componentHandleServices_.end();++it) {
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

BGE::ComponentService::ComponentService(SpaceHandle spaceHandle) : Service(), spaceHandle_(spaceHandle) {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
    }
    
    // Create our componentHandle vectors
    for (auto it=componentHandleServices_.begin();it!=componentHandleServices_.end();++it) {
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

BGE::ComponentService::~ComponentService() {
}

BGE::GameObject *BGE::ComponentService::getComponentGameObject(Component *component, GameObjectHandle gameObjHandle) {
    return component->getSpace()->getGameObject(gameObjHandle);
}

void BGE::ComponentService::removeComponent(ComponentHandle handle) {
    auto typeId = handle.typeId;
    auto &handles = componentHandles_[typeId];
    
    for (auto h=handles.begin();h!=handles.end();++h) {
        if (h->handle == handle.handle) {
            if (typeId == TransformComponent::typeId_) {
                releaseComponentHandle<TransformComponent>(handle);
            } else if (typeId == BoundingBoxComponent::typeId_) {
                releaseComponentHandle<BoundingBoxComponent>(handle);
            } else if (typeId == AnimationChannelComponent::typeId_) {
                releaseComponentHandle<AnimationChannelComponent>(handle);
            } else if (typeId == AnimationSequenceComponent::typeId_) {
                releaseComponentHandle<AnimationSequenceComponent>(handle);
            } else if (typeId == AnimatorComponent::typeId_) {
                releaseComponentHandle<AnimatorComponent>(handle);
            } else if (typeId == ChannelFrameAnimatorComponent::typeId_) {
                releaseComponentHandle<ChannelFrameAnimatorComponent>(handle);
            } else if (typeId == ColorMatrixComponent::typeId_) {
                releaseComponentHandle<ColorMatrixComponent>(handle);
            } else if (typeId == ColorTransformComponent::typeId_) {
                releaseComponentHandle<ColorTransformComponent>(handle);
            } else if (typeId == FrameAnimatorComponent::typeId_) {
                releaseComponentHandle<FrameAnimatorComponent>(handle);
            } else if (typeId == SpriteRenderComponent::typeId_) {
                releaseComponentHandle<SpriteRenderComponent>(handle);
            } else if (typeId == TextComponent::typeId_) {
                releaseComponentHandle<TextComponent>(handle);
            } else if (typeId == ButtonComponent::typeId_) {
                releaseComponentHandle<ButtonComponent>(handle);
            } else if (typeId == InputTouchComponent::typeId_) {
                releaseComponentHandle<InputTouchComponent>(handle);
            } else if (typeId == LineRenderComponent::typeId_) {
                releaseComponentHandle<LineRenderComponent>(handle);
            } else if (typeId == FlatRectRenderComponent::typeId_) {
                releaseComponentHandle<FlatRectRenderComponent>(handle);
            } else if (typeId == MaskComponent::typeId_) {
                releaseComponentHandle<MaskComponent>(handle);
            } else if (typeId == TextureMaskComponent::typeId_) {
                releaseComponentHandle<TextureMaskComponent>(handle);
            } else {
                assert(false);
            }

            handles.erase(h);
            return;
        }
    }
}

