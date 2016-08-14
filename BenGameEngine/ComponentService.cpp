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

void BGE::ComponentService::registerComponents() {
    Component::registerBitmask<AnimationChannelComponent>();
    Component::registerBitmask<AnimationSequenceComponent>();
    Component::registerBitmask<AnimatorComponent>();
    Component::registerBitmask<ChannelFrameAnimatorComponent>();
    Component::registerBitmask<ColorMatrixComponent>();
    Component::registerBitmask<ColorTransformComponent>();
    Component::registerBitmask<FlatRectRenderComponent>();
    Component::registerBitmask<FrameAnimatorComponent>();
    Component::registerBitmask<LineRenderComponent>();
    Component::registerBitmask<SpriteRenderComponent>();
    Component::registerBitmask<TextComponent>();
    Component::registerBitmask<TransformComponent>();
    Component::registerBitmask<ButtonComponent>();
    Component::registerBitmask<MaskComponent>();
    Component::registerBitmask<TextureMaskComponent>();
    Component::registerBitmask<InputTouchComponent>();
    Component::registerBitmask<BoundingBoxComponent>();
}

BGE::ComponentService::ComponentService() {
}

BGE::ComponentService::ComponentService(SpaceHandle spaceHandle) : spaceHandle_(spaceHandle) {
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

