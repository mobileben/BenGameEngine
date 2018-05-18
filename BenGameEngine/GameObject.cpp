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
#include "LogicComponent.h"

BGE::GameObject::GameObject() : NamedObject(), active_(false), componentBitmask_(0) {
}

BGE::GameObject::GameObject(ObjectId objId) : NamedObject(objId), active_(false), componentBitmask_(0) {
}

void BGE::GameObject::initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, const std::string& name) {
    setName(name);

    active_ = false;

    removeAllComponents();

    handle_ = gameObjHandle;
    spaceHandle_ = spaceHandle;
}

void BGE::GameObject::markForDestroy() {
    if (!handle_.isNull()) {
        active_ = false;
        destroy_ = true;

        // Now tag all children to be destroyed
        auto xform = getComponent<TransformComponent>();
        if (xform) {
            for (auto &childXform : xform->getChildren()) {
                auto child = childXform->getGameObject();
                if (child) {
                    child->markForDestroy();
                }
            }
        }
    }
}

void BGE::GameObject::destroy() {
    active_ = false;

    removeAllComponents();
    
    handle_ = GameObjectHandle();
    spaceHandle_ = SpaceHandle();
}

void BGE::GameObject::listComponents() const {
    printf("GameObj %s has components", getName().c_str());
    for (auto it = components_.begin();it != components_.end();++it) {
        printf(" %d", it->typeId);
    }
    printf("\n");
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

BGE::GameObject *BGE::GameObject::find(ComponentTypeId componentTypeId, const std::string& name) {
    if (getName() == name) {
        for (auto &componentHandle : components_) {
            if (componentHandle.typeId == componentTypeId) {
                return this;
            }
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

BGE::GameObject *BGE::GameObject::findWithPrefix(ComponentTypeId componentTypeId, const std::string& name) {
    if (getName().find(name) != std::string::npos) {
        for (auto &componentHandle : components_) {
            if (componentHandle.typeId == componentTypeId) {
                return this;
            }
        }
    }
    
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        for (auto &childXform : xform->getChildren()) {
            auto child = childXform->getGameObject();
            
            if (child) {
                auto found = child->findWithPrefix(componentTypeId, name);
                
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

bool BGE::GameObject::isClipped(void) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->isClipped();
    }
    
    return false;
}

void BGE::GameObject::setClipped(bool clipped) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->setClipped(clipped);
    }
}

bool BGE::GameObject::canRender() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->canRender();
    }
    
    return false;
}

bool BGE::GameObject::canInteract() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->canInteract();
    }
    
    return false;
}

BGE::GameObject * BGE::GameObject::getParent() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        auto parentXform = xform->getParent();
        
        if (parentXform) {
            return parentXform->getGameObject();
        }
    }
    
    return nullptr;
}

bool BGE::GameObject::hasChildren() {
    auto xform = getComponent<TransformComponent>();

    if (xform) {
        return xform->getNumChildren() != 0;
    }
    return false;
}

uint32_t BGE::GameObject::numChildren() {
    auto xform = getComponent<TransformComponent>();

    if (xform) {
        xform->getNumChildren();
    }
    return 0;
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

void BGE::GameObject::moveToParent(GameObjectHandle handle) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->moveToParent(handle);
    }
}

void BGE::GameObject::moveToParent(GameObject *parent) {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->moveToParent(parent);
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

void BGE::GameObject::addComponentEpilogue(ComponentTypeId componentTypeId) {
    if (componentTypeId == LogicComponent::typeId_) {
        Game::getInstance()->getLogicService()->addGameObject(this);
    }
}

void BGE::GameObject::removeComponentPrologue(ComponentTypeId componentTypeId) {
    if (componentTypeId == LogicComponent::typeId_) {
        Game::getInstance()->getLogicService()->removeGameObject(this);
    }
}

bool BGE::GameObject::isAnimationSubSequence() const {
    return hasComponent<AnimationSequenceComponent>() && (hasComponent<FrameAnimatorComponent>() || hasComponent<ChannelFrameAnimatorComponent>());
}

void BGE::GameObject::upgradeToAnimationSequence() {
    if (isAnimationSubSequence()) {
        if (!hasComponent<AnimatorComponent>()) {
            auto space = getSpace();
            auto animator = space->createComponent<AnimatorComponent>();

            addComponent(animator);

            auto bbox = getComponent<BoundingBoxComponent>();
            
            if (!bbox) {
                bbox = space->createComponent<BoundingBoxComponent>();
                addComponent(bbox);
            }
            
            animator->setFrame(0, true);
        }
    }
}

