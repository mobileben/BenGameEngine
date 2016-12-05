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

BGE::TransformComponent::TransformComponent() : Component(), visible_(true), clipped_(false),
bounds_({ 0, 0, 0, 0}), useCollisionRectScale_(false), collisionRectScale_({1, 1}), position_({ 0, 0 }), z_(0), scale_( { 1, 1 }), skew_({ 0, 0 }), useSkew_(false), rotation_(0), localDirty_(false), worldDirty_(false), speed_(1), paused_(false) {
    Matrix4MakeIdentify(localMatrix_);
    Matrix4MakeIdentify(worldMatrix_);
}

void BGE::TransformComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);

    visible_ = true;
    clipped_ = false;
    
    bounds_.x = 0;
    bounds_.y = 0;
    bounds_.w = 0;
    bounds_.h = 0;
    collisionRectScale_.x = 1;
    collisionRectScale_.y = 1;
    useCollisionRectScale_ = false;
    position_.x = 0;
    position_.y = 0;
    scale_.x = 1;
    scale_.y = 1;
    skew_.x = 0;
    skew_.y = 0;
    useSkew_ = false;
    rotation_ = 0;
    
    
    localDirty_ = false;
    worldDirty_ = false;
    
    speed_ = 1;
    paused_ = false;
    
    Matrix4MakeIdentify(localMatrix_);
    Matrix4MakeIdentify(worldMatrix_);
}

void BGE::TransformComponent::destroy() {
    visible_ = false;
    clipped_ = false;

    removeFromParent();
    removeAllChildren();
    
    parentHandle_ = TransformComponentHandle();
    childrenHandles_.clear();

    // Component::destroy last
    Component::destroy();
}

void BGE::TransformComponent::setParentHandle(TransformComponentHandle parentHandle) {
    parentHandle_ = parentHandle;
    worldDirty_ = true;
}

void BGE::TransformComponent::setParent(TransformComponent *parent) {
    if (parent) {
        setParentHandle(parent->getHandle<TransformComponent>());
    }
}

void BGE::TransformComponent::setCollisionRectScale(Vector2 &scale) {
    collisionRectScale_ = scale;
    
    if (scale.x != 1.0 || scale.y != 1.0) {
        useCollisionRectScale_ = true;
    } else {
        useCollisionRectScale_ = false;
    }
}

void BGE::TransformComponent::setCollisionRectScale(Vector2 &&scale) {
    collisionRectScale_ = scale;
    
    if (scale.x != 1.0 || scale.y != 1.0) {
        useCollisionRectScale_ = true;
    } else {
        useCollisionRectScale_ = false;
    }
}

void BGE::TransformComponent::setX(float x) {
    if (position_.x != x) {
        position_.x = x;

        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setY(float y) {
    if (position_.y != y) {
        position_.y = y;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setZ(float z) {
    if (z_ != z) {
        z_ = z;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setPosition(Vector2 &position) {
    if (position_.x != position.x || position_.y != position.y) {
        position_ = position;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setPosition(Vector2 &&position) {
    if (position_.x != position.x || position_.y != position.y) {
        position_ = position;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setScaleX(float x) {
    if (scale_.x != x) {
        scale_.x = x;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setScaleY(float y) {
    if (scale_.y != y) {
        scale_.y = y;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setScale(Vector2 &scale) {
    if (scale_.x != scale.x || scale_.y != scale.y) {
        scale_ = scale;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setScale(Vector2 &&scale) {
    if (scale_.x != scale.x || scale_.y != scale.y) {
        scale_ = scale;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setSkewX(float x) {
    if (skew_.x != x) {
        skew_.x = x;
        
        if (skew_.x != 0 || skew_.y != 0) {
            useSkew_ = true;
        } else {
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setSkewY(float y) {
    if (skew_.y != y) {
        skew_.y = y;
        
        if (skew_.x != 0 || skew_.y != 0) {
            useSkew_ = true;
        } else {
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setSkew(Vector2 &skew) {
    if (skew_.x != skew.x || skew_.y != skew.y) {
        skew_ = skew;
        
        if (skew_.x != 0 || skew_.y != 0) {
            useSkew_ = true;
        } else {
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setSkew(Vector2 &&skew) {
    if (skew_.x != skew.x || skew_.y != skew.y) {
        skew_ = skew;
        
        if (skew_.x != 0 || skew_.y != 0) {
            useSkew_ = true;
        } else {
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::setRotationInDegrees(float rotation) {
    float radians = rotation * (M_PI/180.0);
    
    setRotationInRadians(radians);
}

void BGE::TransformComponent::setRotationInRadians(float rotation) {
    if (rotation_ != rotation) {
        rotation_ = rotation;
        
        localDirty_ = worldDirty_ = true;
    }
}

void BGE::TransformComponent::forceDirty() {
    worldDirty_ = true;
}

void BGE::TransformComponent::updateMatrix() {
    // If our local xform is dirty, compute
    if (localDirty_) {
        Matrix4 mat1;
        Matrix4 mat2;

        Matrix4MakeScale(mat1, scale_.x, scale_.y, 1);

        if (useSkew_) {
            Matrix4MakeFlashSkew(mat2, skew_.x, skew_.y);
        } else {
            Matrix4MakeRotationZ(mat2, rotation_);
        }
        
        localMatrix_ = mat2 * mat1;
        Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
        localMatrix_ = mat1 * localMatrix_;
    }

    auto parent = getParent();
    
    if (parent) {
        parent->updateMatrix();
        worldMatrix_ = parent->worldMatrix_ * localMatrix_;
    } else {
        worldMatrix_ = localMatrix_;
    }
    
    localDirty_ = worldDirty_ = false;
}

void BGE::TransformComponent::updateMatrixAndChildren(bool parentDirty) {
    auto space = getSpace();
    
    parentDirty = parentDirty || localDirty_ || worldDirty_;
    
    // If our local xform is dirty, compute
    if (localDirty_) {
        Matrix4 mat1;
        Matrix4 mat2;
        
        Matrix4MakeScale(mat1, scale_.x, scale_.y, 1);
        
        if (useSkew_) {
            Matrix4MakeFlashSkew(mat2, skew_.x, skew_.y);
        } else {
            Matrix4MakeRotationZ(mat2, rotation_);
        }
        
        localMatrix_ = mat2 * mat1;
        Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
        localMatrix_ = mat1 * localMatrix_;
    }
    
    if (parentDirty) {
        auto parent = getParent();
        
        if (parent) {
            worldMatrix_ = parent->worldMatrix_ * localMatrix_;
        } else {
            worldMatrix_ = localMatrix_;
        }
    }

    for (auto childHandle : childrenHandles_) {
        auto child = space->getComponent(childHandle);
        
        child->updateMatrixAndChildren(parentDirty);
    }
    
    localDirty_ = worldDirty_ = false;
}

BGE::Vector2 BGE::TransformComponent::getGlobalPosition() {
    Matrix4 mtx;
    Vector2 results;
    
    getWorldMatrix(mtx);
    
    Matrix4MultiplyVector2(results, mtx, Vector2{0, 0});
                           
    return results;
}

void BGE::TransformComponent::getWorldMatrix(Matrix4 &matrix) {
    updateMatrix();
    
    matrix = worldMatrix_;
}

const float *BGE::TransformComponent::getWorldMatrixRaw() {
    updateMatrix();

    return worldMatrix_.m;
}


void BGE::TransformComponent::getLocalMatrix(Matrix4 &matrix) {
    updateMatrix();
    
    matrix = localMatrix_;
}

const float *BGE::TransformComponent::getLocalMatrixRaw() {
    updateMatrix();
    
    return localMatrix_.m;
}

BGE::TransformComponent *BGE::TransformComponent::getParent() const {
    auto space = getSpace();
    
    return space->getComponent<TransformComponent>(parentHandle_.getHandle());
}

std::vector<BGE::TransformComponentHandle> BGE::TransformComponent::getOrderedChildrenHandles() {
    handleServicesLock();
    
    auto xforms = getOrderedChildren();
    std::vector<TransformComponentHandle> children;

    for (auto xform : xforms) {
        children.push_back(xform->getHandle<TransformComponent>());
    }

    handleServicesUnlock();

    return children;
}

std::vector<BGE::TransformComponentHandle> BGE::TransformComponent::getReverseOrderedChildrenHandles() {
    handleServicesLock();
    
    auto xforms = getReverseOrderedChildren();
    std::vector<TransformComponentHandle> children;
    
    for (auto xform : xforms) {
        children.push_back(xform->getHandle<TransformComponent>());
    }
    
    handleServicesUnlock();
    
    return children;
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

std::vector<BGE::TransformComponent *> BGE::TransformComponent::getOrderedChildren() {
    handleServicesLock();

    auto space = getSpace();
    std::vector<TransformComponent *> children;
    
    for (auto handle : childrenHandles_) {
        auto xform = space->getComponent<TransformComponent>(handle.getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    TransformComponent **xforms = &children[0];
    
    int32_t i, j;
    
    for (i=1;i<children.size();++i) {
        j = i;
        
        while (j > 0 && xforms[j]->getZ() < xforms[j-1]->getZ()) {
            auto temp = xforms[j];
            xforms[j] = xforms[j-1];
            xforms[j-1] = temp;
            --j;
        }
    }
    
    handleServicesUnlock();

    return children;
}

std::vector<BGE::TransformComponent *> BGE::TransformComponent::getReverseOrderedChildren() {
    handleServicesLock();
    
    auto space = getSpace();
    std::vector<TransformComponent *> children;
    
    for (int32_t i=static_cast<int32_t>(childrenHandles_.size())-1;i>=0;--i) {
        auto xform = space->getComponent<TransformComponent>(childrenHandles_[i].getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    TransformComponent **xforms = &children[0];
    
    int32_t i, j;
    
    for (i=1;i<children.size();++i) {
        j = i;
        
        while (j > 0 && xforms[j]->getZ() > xforms[j-1]->getZ()) {
            auto temp = xforms[j];
            xforms[j] = xforms[j-1];
            xforms[j-1] = temp;
            --j;
        }
    }
    
    handleServicesUnlock();
    
    return children;
}

void BGE::TransformComponent::addChild(TransformComponentHandle handle) {
    auto space = getSpace();
    auto child = space->getComponent<TransformComponent>(handle.getHandle());
    assert(child);
    assert(!child->getParent());

    if  (child) {
        childrenHandles_.push_back(handle);
        child->setParent(this);
    }
}

void BGE::TransformComponent::addChild(TransformComponent *child) {
    assert(child);
    assert(!child->getParent());    // Child cannot already have a parent
    
    if (child) {
        auto handle = child->getHandle<TransformComponent>();
        childrenHandles_.push_back(handle);
        child->setParent(this);
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
                    otherXform->setParent(this);
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
        child->setParentHandle(TransformComponentHandle());
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
    }
    
    setParentHandle(TransformComponentHandle());
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
        
        child->setParent(this);
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
        
        child->setParent(this);
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
            
            childXform->setParent(this);
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
            
            childXform->setParent(this);
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
        
        child->setParent(this);
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
        
        child->setParent(this);
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
