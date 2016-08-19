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
        registerComponent<TransformComponent, TransformComponentHandle>();
        registerComponent<BoundingBoxComponent, BoundingBoxComponentHandle>();
        registerComponent<AnimationChannelComponent, AnimationChannelComponentHandle>();
        registerComponent<AnimationSequenceComponent, AnimationSequenceComponentHandle>();
        registerComponent<AnimatorComponent, AnimatorComponentHandle>();
        registerComponent<ChannelFrameAnimatorComponent, ChannelFrameAnimatorComponentHandle>();
        registerComponent<ColorMatrixComponent, ColorMatrixComponentHandle>();
        registerComponent<ColorTransformComponent, ColorTransformComponentHandle>();
        registerComponent<FrameAnimatorComponent, FrameAnimatorComponentHandle>();
        registerComponent<SpriteRenderComponent, SpriteRenderComponentHandle>();
        registerComponent<TextComponent, TextComponentHandle>();
        registerComponent<ButtonComponent, ButtonComponentHandle>();
        registerComponent<InputTouchComponent, InputTouchComponentHandle>();
        registerComponent<LineRenderComponent, LineRenderComponentHandle>();
        registerComponent<FlatRectRenderComponent, FlatRectRenderComponentHandle>();
        registerComponent<MaskComponent, MaskComponentHandle>();
        registerComponent<TextureMaskComponent, TextureMaskComponentHandle>();
    }
    
    componentsRegistered_ = true;
}

BGE::ComponentService::ComponentService() : Service(), foo_(10, 10) {
    for (auto &creator : handleServiceCreators_) {
        componentHandleServices_.push_back(static_cast<void *>(creator(120, 120)));
        
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

BGE::ComponentService::ComponentService(SpaceHandle spaceHandle) : Service(), foo_(10, 10), spaceHandle_(spaceHandle) {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
    }
}

BGE::ComponentService::~ComponentService() {
}

template <typename T>
std::shared_ptr<T> BGE::ComponentService::getComponent(ObjectId componentId) {
    std::type_index index(typeid(T));
    ComponentMapIterator it = components_.find(index);
    
    if (it != components_.end()) {
        for (auto component : it->second) {
            if (component->getInstanceId() == componentId) {
                return component;
            }
        }
    }
    
    return nullptr;
}

template <typename T>
void BGE::ComponentService::removeComponent(ObjectId componentId) {
    std::type_index index(typeid(T));
    ComponentMapIterator it = components_.find(index);
    
    if (it != components_.end()) {
        ComponentVector vec = it->second;
        
        for (ComponentVectorIterator vit=vec.begin();vit != vec.end();vit++) {
            if ((*vit)->getInstanceId() == componentId) {
                vec.erase(vit);
                return;
            }
        }
    }
}

template <typename T>
void BGE::ComponentService::removeAllComponents() {
    components_.erase(typeid(T));
}

void BGE::ComponentService::removeComponent(std::type_index typeIndex, ObjectId componentId) {
    ComponentMapIterator it = components_.find(typeIndex);
    
    if (it != components_.end()) {
        ComponentVector vec = it->second;
        
        for (ComponentVectorIterator vit=vec.begin();vit != vec.end();vit++) {
            if ((*vit)->getInstanceId() == componentId) {
                vec.erase(vit);
                return;
            }
        }
    }
}

BGE::GameObject *BGE::ComponentService::getComponentGameObject(Component *component, GameObjectHandle gameObjHandle) {
    return component->getSpace()->getGameObject(gameObjHandle);
}

