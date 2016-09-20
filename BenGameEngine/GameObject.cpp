//
//  GameObject.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GameObject.h"
#include "Space.h"
#include "Game.h"
#include "ComponentBitmask.h"
#include "AnimationSequenceComponent.h"
#include "BoundingBoxComponent.h"
#include "AnimatorComponent.h"
#include "FrameAnimatorComponent.h"
#include "TransformComponent.h"

BGE::GameObject::GameObject() : NamedObject(), active_(false), componentBitmask_(0) {
}

BGE::GameObject::GameObject(ObjectId objId) : NamedObject(objId), active_(false), componentBitmask_(0) {
}

void BGE::GameObject::initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, std::string name) {
    setName(name);

    active_ = false;

    removeAllComponents();

    handle_ = gameObjHandle;
    spaceHandle_ = spaceHandle;
}

void BGE::GameObject::destroy() {
    active_ = false;

    removeAllComponents();
    
    handle_ = GameObjectHandle();
    spaceHandle_ = SpaceHandle();
}

void BGE::GameObject::removeAllComponents() {
    auto space = getSpace();
    
    if (space) {
        for (auto const &component : components_) {
            space->removeComponent(component);
        }
    }
    
    componentBitmask_ = 0;
    components_.clear();
}

BGE::Space *BGE::GameObject::getSpace() const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}

BGE::GameObject *BGE::GameObject::find(ComponentTypeId componentTypeId, std::string name) {
    for (auto &componentHandle : components_) {
        if (componentHandle.typeId == componentTypeId && getName() == name) {
            return this;
        }
    }
    
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        for (auto &childXform : xform->getChildren()) {
            auto child = childXform->getGameObject();
            
            if (child) {
                auto found = child->find(componentTypeId, name);
                
                if (found) {
                    return found;
                }
            }
        }
    }
    
    return nullptr;
}

bool BGE::GameObject::isVisible(void) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->isVisible();
    }
    
    return false;
}

void BGE::GameObject::setVisibility(bool visible) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->setVisibility(visible);
    }
}

void BGE::GameObject::addChild(GameObjectHandle handle) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->addChild(handle);
    }
}

void BGE::GameObject::addChild(GameObject *child) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->addChild(child);
    }
}

void BGE::GameObject::insertChild(GameObjectHandle handle, uint32_t index) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->insertChild(handle, index);
    }
}

void BGE::GameObject::insertChild(GameObject *child, uint32_t index) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->insertChild(child, index);
    }
}

void BGE::GameObject::replaceChild(GameObjectHandle handle, uint32_t index) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->replaceChild(handle, index);
    }
}

void BGE::GameObject::replaceChild(GameObject *child, uint32_t index) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->replaceChild(child, index);
    }
}

void BGE::GameObject::removeAllChildren() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->removeAllChildren();
    }
}

void BGE::GameObject::removeFromParent() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->removeFromParent();
    }
}

bool BGE::GameObject::isAnimationSubSequence() const {
    return hasComponent<AnimationSequenceComponent>() && hasComponent<FrameAnimatorComponent>();
}

void BGE::GameObject::upgradeToAnimationSequence() {
    if (isAnimationSubSequence()) {
        if (!hasComponent<AnimatorComponent>()) {
            auto space = getSpace();
            auto boundingBox = space->createComponent<BoundingBoxComponent>();
            auto animator = space->createComponent<AnimatorComponent>();
            
            addComponent(boundingBox);
            addComponent(animator);
            
            animator->setFrame(0, true);
        }
    }
}

