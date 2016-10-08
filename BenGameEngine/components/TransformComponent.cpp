//
//  TransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TransformComponent.h"
#include "ComponentBitmask.h"
#include "Space.h"
#include "GameObject.h"
#include <cassert>

uint32_t BGE::TransformComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::TransformComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::TransformComponent::type_index_ = typeid(BGE::TransformComponent);

BGE::TransformComponent::TransformComponent() : Component(), visible_(true),
bounds_({ 0, 0, 0, 0}), position_({ 0, 0 }), z_(0), scale_( { 1, 1 }), skew_({ 0, 0 }), rotation_(0), transformDirty_(false), speed_(1), paused_(false) {
    Matrix4MakeIdentify(worldMatrix_);
}

void BGE::TransformComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);

    visible_ = true;
    
    bounds_.x = 0;
    bounds_.y = 0;
    bounds_.w = 0;
    bounds_.h = 0;
    position_.x = 0;
    position_.y = 0;
    scale_.x = 1;
    scale_.y = 1;
    skew_.x = 0;
    skew_.y = 0;
    rotation_ = 0;
    transformDirty_ = false;
    speed_ = 1;
    paused_ = false;
    
    Matrix4MakeIdentify(worldMatrix_);
}

void BGE::TransformComponent::destroy() {
    visible_ = false;
    
    removeFromParent();
    removeAllChildren();
    
    parentHandle_ = TransformComponentHandle();
    childrenHandles_.clear();

    // Component::destroy last
    Component::destroy();
}

void BGE::TransformComponent::setGameObjectHandle(GameObjectHandle handle) {
    Component::setGameObjectHandle(handle);
}

void BGE::TransformComponent::updateMatrix() {
    Matrix4 mat1;
    Matrix4 mat2;
    
    Matrix4MakeScale(mat1, scale_.x, scale_.y, 1);
    Matrix4MakeRotationZ(mat2, rotation_);
    
    localMatrix_ = mat2 * mat1;
    Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
    localMatrix_ = mat1 * localMatrix_;
    
    auto parent = getParent();
    
    if (parent) {
        parent->getWorldMatrix(mat1);
        
        worldMatrix_ = mat1 * localMatrix_;
    } else {
        worldMatrix_ = localMatrix_;
    }
}

BGE::Vector2 BGE::TransformComponent::getGlobalPosition() {
    Matrix4 mtx;
    Vector2 results;
    
    getWorldMatrix(mtx);
    
    Matrix4MultiplyVector2(results, mtx, position_);
                           
    return results;
}

void BGE::TransformComponent::getWorldMatrix(Matrix4 &matrix) {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    matrix = worldMatrix_;
}

const float *BGE::TransformComponent::getWorldMatrixRaw() {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    return worldMatrix_.m;
}


void BGE::TransformComponent::getLocalMatrix(Matrix4 &matrix) {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    matrix = localMatrix_;
}

const float *BGE::TransformComponent::getLocalMatrixRaw() {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    return localMatrix_.m;
}

BGE::TransformComponent *BGE::TransformComponent::getParent() const {
    auto space = getSpace();
    
    return space->getComponent<TransformComponent>(parentHandle_.getHandle());
}

std::vector<BGE::TransformComponent *> BGE::TransformComponent::getChildren() {
    auto space = getSpace();
    std::vector<TransformComponent *> children;
    
    for (auto handle : childrenHandles_) {
        auto xform = space->getComponent<TransformComponent>(handle.getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    return children;
}

void BGE::TransformComponent::addChild(TransformComponentHandle handle) {
    auto space = getSpace();
    auto child = space->getComponent<TransformComponent>(handle.getHandle());
    assert(child);
    assert(!child->getParent());

    if  (child) {
        childrenHandles_.push_back(handle);
        child->parentHandle_ = getHandle<TransformComponent>();
    }
}

void BGE::TransformComponent::addChild(TransformComponent *child) {
    assert(child);
    assert(!child->getParent());    // Child cannot already have a parent
    
    if (child) {
        auto handle = child->getHandle<TransformComponent>();
        childrenHandles_.push_back(handle);
        child->parentHandle_ = getHandle<TransformComponent>();
        
        // TODO: Update hierarchy
    }
}

void BGE::TransformComponent::addChild(GameObjectHandle handle) {
    auto space = getSpace();
    
    if (space) {
        // Get GameObject from space so we don't get space twice (this->getGameObject would do it)
        auto gameObj = space->getGameObject(getGameObjectHandle());
        
        if (gameObj) {
            auto otherGameObj = space->getGameObject(handle);
            
            if (otherGameObj) {
                auto otherXform = otherGameObj->getComponent<TransformComponent>();
                
                if (otherXform) {
                    assert(!otherXform->getParent());
                    
                    childrenHandles_.push_back(otherXform->getHandle<TransformComponent>());
                    otherXform->parentHandle_ = getHandle<TransformComponent>();
                }
            }
        }
    }
}

void BGE::TransformComponent::addChild(GameObject *child) {
    assert(child);
    
    if (child) {
        auto xform = child->getComponent<TransformComponent>();
        
        addChild(xform);
    }
}

void BGE::TransformComponent::removeAllChildren() {
    auto space = getSpace();
    
    for (auto handle : childrenHandles_) {
        auto child = space->getComponent<TransformComponent>(handle.getHandle());
        // Remove parent from all children
        child->parentHandle_ = TransformComponentHandle();
    }
    
    childrenHandles_.clear();
}

void BGE::TransformComponent::removeFromParent() {
    auto space = getSpace();
    auto parent = space->getComponent<TransformComponent>(parentHandle_.getHandle());

    if (parent) {
        for (auto it = parent->childrenHandles_.begin();it != parent->childrenHandles_.end();++it) {
            if (it->getHandle() == getRawHandle()) {
                parent->childrenHandles_.erase(it);
                break;
            }
        }
        
        parentHandle_ = TransformComponentHandle();
    } else {
        parentHandle_ = TransformComponentHandle();
    }
}

void BGE::TransformComponent::insertChild(TransformComponentHandle handle, uint32_t index) {
    auto space = getSpace();
    auto child = space->getComponent<TransformComponent>(handle.getHandle());
    
    assert(child);
    assert(!child->getParent());    // Child cannot already have a parent
    
    if (child) {
        if (index < childrenHandles_.size()) {
            childrenHandles_.insert(childrenHandles_.begin() + index, child->getHandle<TransformComponent>());
        } else {
            childrenHandles_.push_back(child->getHandle<TransformComponent>());
        }
        
        child->parentHandle_ = getHandle<TransformComponent>();
    }
}

void BGE::TransformComponent::insertChild(TransformComponent *child, uint32_t index) {
    assert(child);
    assert(!child->getParent());
    
    if (child) {
        if (index < childrenHandles_.size()) {
            childrenHandles_.insert(childrenHandles_.begin() + index, child->getHandle<TransformComponent>());
        } else {
            childrenHandles_.push_back(child->getHandle<TransformComponent>());
        }
        
        child->parentHandle_ = getHandle<TransformComponent>();
    }
}

void BGE::TransformComponent::insertChild(GameObjectHandle handle, uint32_t index) {
    auto space = getSpace();
    auto child = space->getGameObject(handle);
    
    assert(child);
    
    if (child) {
        auto childXform = child->getComponent<TransformComponent>();
        assert(!childXform->getParent());    // Child cannot already have a parent
        
        if (childXform) {
            if (index < childrenHandles_.size()) {
                childrenHandles_.insert(childrenHandles_.begin() + index, childXform->getHandle<TransformComponent>());
            } else {
                childrenHandles_.push_back(childXform->getHandle<TransformComponent>());
            }
            
            childXform->parentHandle_ = getHandle<TransformComponent>();
        }
    }
}

void BGE::TransformComponent::insertChild(GameObject *child, uint32_t index) {
    assert(child);
    
    if (child) {
        auto childXform = child->getComponent<TransformComponent>();
        assert(!childXform->getParent());    // Child cannot already have a parent

        if (childXform) {
            if (index < childrenHandles_.size()) {
                childrenHandles_.insert(childrenHandles_.begin() + index, childXform->getHandle<TransformComponent>());
            } else {
                childrenHandles_.push_back(childXform->getHandle<TransformComponent>());
            }
            
            childXform->parentHandle_ = getHandle<TransformComponent>();
        }
    }
}

void BGE::TransformComponent::replaceChild(TransformComponentHandle handle, uint32_t index) {
    auto space = getSpace();
    auto child = space->getComponent<TransformComponent>(handle.getHandle());
    auto hasParent = child->getParent() != nullptr;
    
    assert(child);
    assert(!hasParent);    // Child cannot already have a parent
    assert(index < childrenHandles_.size());
    
    if (child && index < childrenHandles_.size()) {
        auto existing = space->getComponent<TransformComponent>(childrenHandles_[index]);
        
        if (hasParent) {
            child->removeFromParent();
        }
        
        existing->removeFromParent();
        
        child->parentHandle_ = getHandle<TransformComponent>();
        childrenHandles_[index] = child->getHandle<TransformComponent>();
    }
}

void BGE::TransformComponent::replaceChild(TransformComponent *child, uint32_t index) {
    auto space = getSpace();
    auto hasParent = child->getParent() != nullptr;
    
    assert(child);
    assert(!hasParent);    // Child cannot already have a parent
    assert(index < childrenHandles_.size());
    
    if (child && index < childrenHandles_.size()) {
        auto existing = space->getComponent<TransformComponent>(childrenHandles_[index]);
        
        if (hasParent) {
            child->removeFromParent();
        }
        
        existing->removeFromParent();
        
        child->parentHandle_ = getHandle<TransformComponent>();
        childrenHandles_[index] = child->getHandle<TransformComponent>();
    }
}

void BGE::TransformComponent::replaceChild(GameObjectHandle handle, uint32_t index) {
    auto space = getSpace();
    auto gameObj = space->getGameObject(handle);
    auto child = gameObj->getComponent<TransformComponent>();
    
    replaceChild(child, index);
}

void BGE::TransformComponent::replaceChild(GameObject *child, uint32_t index) {
    auto xform = child->getComponent<TransformComponent>();
    
    replaceChild(xform, index);
}

void BGE::TransformComponent::moveToParent(TransformComponentHandle handle) {
    auto space = getSpace();
    auto parent = space->getComponent<TransformComponent>(handle.getHandle());
    
    assert(parent);
    
    if (parent) {
        removeFromParent();
        
        parent->addChild(this);
    }
}

void BGE::TransformComponent::moveToParent(TransformComponent *parent) {
    assert(parent);
    
    if (parent) {
        removeFromParent();
        
        parent->addChild(this);
    }
}

void BGE::TransformComponent::moveToParent(GameObjectHandle handle) {
    auto space = getSpace();
    auto parent = space->getGameObject(handle);
    
    assert(parent);
    
    if (parent) {
        auto parentXform = parent->getComponent<TransformComponent>();
        
        assert(parentXform);
        
        if (parentXform) {
            removeFromParent();
            
            parentXform->addChild(this);
        }
    }
}

void BGE::TransformComponent::moveToParent(GameObject *parent) {
    assert(parent);
    
    if (parent) {
        auto parentXform = parent->getComponent<TransformComponent>();
        
        assert(parentXform);
        
        if (parentXform) {
            removeFromParent();
        
            parentXform->addChild(this);
        }
    }
}

BGE::TransformComponent *BGE::TransformComponent::childAtIndex(uint32_t index) {
    if (index < childrenHandles_.size()) {
        auto handle = childrenHandles_.at(index);
        auto space = getSpace();
        return space->getComponent<TransformComponent>(handle.getHandle());
    } else {
        return nullptr;
    }
}

BGE::TransformComponentHandle BGE::TransformComponent::childHandleAtIndex(uint32_t index) {
    if (index < childrenHandles_.size()) {
        return childrenHandles_.at(index);
    } else {
        return TransformComponentHandle();
    }
}

bool BGE::TransformComponent::hasChild(TransformComponentHandle handle, bool descend) {
    auto space = getSpace();
    auto child = space->getComponent<TransformComponent>(handle.getHandle());
    
    assert(child);
    
    if (child) {
        if (descend) {
            for (auto xformHandle : childrenHandles_) {
                if (xformHandle == handle) {
                    return true;
                } else {
                    auto xform = space->getComponent<TransformComponent>(xformHandle.getHandle());
                    bool found = xform->hasChild(child, descend);
                    
                    if (found) {
                        return found;
                    }
                }
            }
            
            return false;
        } else {
            return std::find(childrenHandles_.begin(), childrenHandles_.end(), handle) != childrenHandles_.end();
        }
    } else {
        return false;
    }
}

bool BGE::TransformComponent::hasChild(TransformComponent *child, bool descend) {
    auto space = getSpace();
    auto handle = getHandle<TransformComponent>();
    
    assert(child);
    
    if (child) {
        if (descend) {
            for (auto xformHandle : childrenHandles_) {
                if (xformHandle == handle) {
                    return true;
                } else {
                    auto xform = space->getComponent<TransformComponent>(xformHandle.getHandle());
                    bool found = xform->hasChild(child, descend);
                    
                    if (found) {
                        return found;
                    }
                }
            }
            
            return false;
        } else {
            return std::find(childrenHandles_.begin(), childrenHandles_.end(), handle) != childrenHandles_.end();
        }
    } else {
        return false;
    }
}

bool BGE::TransformComponent::inParentHierarchy(TransformComponentHandle handle) {
    auto space = getSpace();
    auto parent = space->getComponent<TransformComponent>(handle.getHandle());

    assert(parent);
    
    if (parent) {
        return parent->hasChild(this, true);
    } else {
        return false;
    }
}

bool BGE::TransformComponent::inParentHierarchy(TransformComponent *parent) {
    assert(parent);
    
    if (parent) {
        return parent->hasChild(this, true);
    } else {
        return false;
    }
}
