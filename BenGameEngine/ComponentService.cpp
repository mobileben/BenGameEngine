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
std::vector<std::function<void *(uint32_t, uint32_t)>> BGE::ComponentService::handleServiceCreators_;

void BGE::ComponentService::registerComponents() {
    if (!ComponentService::componentsRegistered_) {
        registerComponent<TransformComponent>();
        registerComponent<BoundingBoxComponent>();
        registerComponent<AnimationChannelComponent>();
        registerComponent<AnimationSequenceComponent>();
        registerComponent<AnimatorComponent>();
        registerComponent<ChannelFrameAnimatorComponent>();
        registerComponent<ColorMatrixComponent>();
        registerComponent<ColorTransformComponent>();
        registerComponent<FrameAnimatorComponent>();
        registerComponent<SpriteRenderComponent>();
        registerComponent<TextComponent>();
        registerComponent<ButtonComponent>();
        registerComponent<InputTouchComponent>();
        registerComponent<LineRenderComponent>();
        registerComponent<FlatRectRenderComponent>();
        registerComponent<MaskComponent>();
        registerComponent<TextureMaskComponent>();
    }
    
    componentsRegistered_ = true;
}

BGE::ComponentService::ComponentService() : Service() {
    for (auto &creator : handleServiceCreators_) {
        componentHandleServices_.push_back(static_cast<void *>(creator(480, 480)));
        
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

BGE::ComponentService::ComponentService(SpaceHandle spaceHandle) : Service(), spaceHandle_(spaceHandle) {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
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

