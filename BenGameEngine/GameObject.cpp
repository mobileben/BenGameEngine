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
#include "ButtonComponent.h"
#include "AnimatorComponent.h"
#include "FrameAnimatorComponent.h"
#include "PolyLineRenderComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "LogicComponent.h"

BGE::GameObject::GameObject() : NamedObject(), active_(false), cachedVisibility_(true), destroy_(false), componentBitmask_(0) {
}

BGE::GameObject::GameObject(ObjectId objId) : NamedObject(objId), active_(false), cachedVisibility_(true), destroy_(false), componentBitmask_(0) {
}

void BGE::GameObject::initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, const std::string& name) {
    setName(name);

    active_ = false;
    cachedVisibility_ = true;
    destroy_ = false;
    
    removeAllComponents();

    handle_ = gameObjHandle;
    spaceHandle_ = spaceHandle;
}

void BGE::GameObject::markForDestroy() {
    if (!handle_.isNull()) {
        active_ = false;
        cachedVisibility_ = false;
        destroy_ = true;

        auto space = getSpace();
        // Now tag all children to be destroyed
        auto xform = getComponent<TransformComponent>(space);
        if (xform) {
            for (auto &childXform : xform->getChildren()) {
                auto child = childXform->getGameObject(space);
                if (child) {
                    child->markForDestroy();
                }
            }
        }
    }
}

void BGE::GameObject::destroy() {
    active_ = false;
    cachedVisibility_ = false;
    
    removeAllComponents();
    
    handle_ = GameObjectHandle();
    spaceHandle_ = SpaceHandle();
}

void BGE::GameObject::destroyDontReleaseComponents() {
    active_ = false;
    cachedVisibility_ = false;

    componentBitmask_ = 0;
    components_.clear();

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

void BGE::GameObject::setActive(bool active) {
    active_ = active;
    
    auto button = getComponent<ButtonComponent>();
    if (button) {
        button->activeUpdated(active);
    }
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
    
    auto space = getSpace();
    auto xform = getComponent<TransformComponent>(space);
    
    if (xform) {
        for (auto &childXform : xform->getChildren()) {
            auto child = childXform->getGameObject(space);
            
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

void BGE::GameObject::setVisibility(bool visible) {
    auto xform = getComponent<TransformComponent>();
    if (xform) {
        xform->setVisibility_(visible);
    }

    cachedVisibility_ = visible;
    
    auto button = getComponent<ButtonComponent>();
    if (button) {
        button->visibilityUpdated(visible);
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
        xform->setClipped(clipped);
    }
}

bool BGE::GameObject::canRender() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        return xform->canRender();
    }
    
    return false;
}

bool BGE::GameObject::canRender(const Space *space) {
    auto xform = getComponent<TransformComponent>(space);
    
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

bool BGE::GameObject::canInteract(const Space *space) {
    auto xform = getComponent<TransformComponent>(space);
    
    if (xform) {
        return xform->canInteract();
    }
    
    return false;
}

bool BGE::GameObject::canInteractLockless(const Space *space) {
    auto xform = getComponentLockless<TransformComponent>(space);
    
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

BGE::GameObject * BGE::GameObject::getParent(const Space *space) {
    auto xform = getComponent<TransformComponent>(space);
    
    if (xform) {
        auto parentXform = xform->getParent(space);
        
        if (parentXform) {
            return parentXform->getGameObject(space);
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

void BGE::GameObject::removeAllChildren(bool destroy) {
    auto xform = getComponent<TransformComponent>();
    if (xform) {
        if (destroy) {
            std::vector<TransformComponentHandle> cHandles = xform->getChildrenHandles();
            xform->removeAllChildren();
            if (cHandles.size()) {
                auto space = getSpace();
                if (space) {
                    for (auto h : cHandles) {
                        auto cXform = space->getComponent<TransformComponent>(h);
                        if (cXform) {
                            space->removeGameObject(cXform->gameObjectHandle_);
                        }
                    }
                }
            }
        } else {
            xform->removeAllChildren();
        }
    }
}

void BGE::GameObject::removeFromParent() {
    auto xform = getComponent<TransformComponent>();
    
    if (xform) {
        xform->removeFromParent();
    }
}

void BGE::GameObject::addComponentEpilogue(ComponentTypeId componentTypeId) {
    if (componentTypeId == TransformComponent::typeId_) {
        auto space = getSpace();
        auto xform = getComponent<TransformComponent>(space);
        if (xform) {
            cachedVisibility_ = xform->isVisible();
        }
    } else if (componentTypeId == LogicComponent::typeId_) {
        Game::getInstance()->getLogicService()->addGameObject(this);
    } else if (componentTypeId == AnimationSequenceComponent::typeId_) {
        auto space = getSpace();
        auto animSeq = getComponent<AnimationSequenceComponent>(space);
        if (animSeq->totalFrames > 1) {
            space->addAnimObject(getHandle());
        }
    } else if (componentTypeId == TextComponent::typeId_) {
        auto space = getSpace();
        auto text = getComponent<TextComponent>(space);
        RenderStringCacheCommandData data(getSpaceHandle(), text->getHandle<TextComponent>());
        Game::getInstance()->getRenderService()->queueCreateStringCacheEntry(data);
    } else if (componentTypeId == PolyLineRenderComponent::typeId_) {
        auto space = getSpace();
        auto poly = getComponent<PolyLineRenderComponent>(space);
        RenderPolyLineCacheCommandData data(getSpaceHandle(), poly->getHandle<PolyLineRenderComponent>());
        Game::getInstance()->getRenderService()->queueCreatePolyLineCacheEntry(data, nullptr);
    }
    
    // Now sort based on type index
    auto size = components_.size();
    if (size > 1) {
        ComponentHandle *handles = &components_[0];
        size_t i, j;
        
        for (i=1;i<size;++i) {
            j = i;
            
            while (j > 0 && handles[j].typeId < handles[j-1].typeId) {
                auto temp = handles[j];
                handles[j] = handles[j-1];
                handles[j-1] = temp;
                --j;
            }
        }
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
            
            animator->setFrame(0);
        }
    }
}

