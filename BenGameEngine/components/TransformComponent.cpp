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
#include "MathTypes.h"
#include <cassert>

uint32_t BGE::TransformComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::TransformComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::TransformComponent::type_index_ = typeid(BGE::TransformComponent);

BGE::TransformComponent::TransformComponent() : Component(), visible_(true), clipped_(false),
bounds_({ 0, 0, 0, 0}), useCollisionRectScale_(false), collisionRectScale_({{1, 1}}), useSkew_(false), position_({ {0, 0} }), z_(0), scale_( { { 1, 1 } }), skew_({ { 0, 0 } }), rotation_(0), localDirty_(false), worldDirty_(false), speed_(1), paused_(false) {
    Matrix4MakeIdentity(localMatrix_);
    Matrix4MakeIdentity(worldMatrix_);
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
    z_ = 0;
    useSkew_ = false;
    rotation_ = 0;
    
    
    localDirty_ = false;
    worldDirty_ = false;
    
    speed_ = 1;
    paused_ = false;
    
    Matrix4MakeIdentity(localMatrix_);
    Matrix4MakeIdentity(worldMatrix_);
}

void BGE::TransformComponent::destroy() {
    visible_ = false;

    removeFromParent();
    removeAllChildren();
    
    parentHandle_ = TransformComponentHandle();
    childrenHandles_.clear();
    sortedChildrenHandles_.clear();

    // Component::destroy last
    Component::destroy();
}

void BGE::TransformComponent::destroyFast() {
    visible_ = false;

    parentHandle_ = TransformComponentHandle();
    childrenHandles_.clear();
    sortedChildrenHandles_.clear();

    // Component::destroyFast last
    Component::destroyFast();
}

void BGE::TransformComponent::setParentHandle(TransformComponentHandle parentHandle) {
    parentHandle_ = parentHandle;
    worldDirty_ = true;
    auto space = getSpace();
    markChildrenWorldMatrixAsDirty(space);
}

void BGE::TransformComponent::setParent(TransformComponent *parent) {
    auto space = getSpace();
    if (parent) {
        setParentHandle(parent->getHandle<TransformComponent>());
        parent->sortChildren(space);
        space->removeRootObject(getGameObjectHandle());
    } else {
        setParentHandle(TransformComponentHandle());
        space->addRootObject(getGameObjectHandle());
    }
}

void BGE::TransformComponent::markChildrenWorldMatrixAsDirty(Space *space) {
    for (auto childHandle : childrenHandles_) {
        auto child = space->getComponent(childHandle);
        child->worldDirty_ = true;
        child->markChildrenWorldMatrixAsDirty(space);
    }
}

void BGE::TransformComponent::setCollisionRectScale(Vector2 &scale) {
    collisionRectScale_ = scale;

    if (BGE::notNearlyEqual(scale.x, 1.0F) || BGE::notNearlyEqual(scale.y, 1.0F)) {
        useCollisionRectScale_ = true;
    } else {
        collisionRectScale_.x = 1.0F;
        collisionRectScale_.y = 1.0F;
        useCollisionRectScale_ = false;
    }
}

void BGE::TransformComponent::setCollisionRectScale(Vector2 &&scale) {
    collisionRectScale_ = scale;

    if (BGE::notNearlyEqual(scale.x, 1.0F) || BGE::notNearlyEqual(scale.y, 1.0F)) {
        useCollisionRectScale_ = true;
    } else {
        collisionRectScale_.x = 1.0F;
        collisionRectScale_.y = 1.0F;
        useCollisionRectScale_ = false;
    }
}

void BGE::TransformComponent::setX(float x) {
    if (BGE::notNearlyEqual(position_.x, x)) {
        if (BGE::nearlyZero(x)) {
            position_.x = 0.0F;
        } else {
            position_.x = x;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setY(float y) {
    if (BGE::notNearlyEqual(position_.y, y)) {
        if (BGE::nearlyZero(y)) {
            position_.y = 0.0F;
        } else {
            position_.y = y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setZ(float z) {
    if (BGE::notNearlyEqual(z_, z)) {
        if (BGE::nearlyZero(z)) {
            z_ = 0.0F;
        } else {
            z_ = z;
        }

        // Z does not affect transforms

        auto space = getSpace();
        auto parent = getParent();
        if (parent) {
            parent->sortChildren(space);
        }
    }
}

void BGE::TransformComponent::setPosition(Vector2 &position) {
    if (BGE::notNearlyEqual(position_.x, position.x) || BGE::notNearlyEqual(position_.y, position.y)) {
        if (BGE::nearlyZero(position.x)) {
            position_.x = 0.0F;
        } else {
            position_.x = position.x;
        }
        if (BGE::nearlyZero(position.y)) {
            position_.y = 0.0F;
        } else {
            position_.y = position.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setPosition(const Vector2 &position) {
    if (BGE::notNearlyEqual(position_.x, position.x) || BGE::notNearlyEqual(position_.y, position.y)) {
        if (BGE::nearlyZero(position.x)) {
            position_.x = 0.0F;
        } else {
            position_.x = position.x;
        }
        if (BGE::nearlyZero(position.y)) {
            position_.y = 0.0F;
        } else {
            position_.y = position.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setPosition(Vector2 &&position) {
    if (BGE::notNearlyEqual(position_.x, position.x) || BGE::notNearlyEqual(position_.y, position.y)) {
        if (BGE::nearlyZero(position.x)) {
            position_.x = 0.0F;
        } else {
            position_.x = position.x;
        }
        if (BGE::nearlyZero(position.y)) {
            position_.y = 0.0F;
        } else {
            position_.y = position.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setPosition(const Vector2 &&position) {
    if (BGE::notNearlyEqual(position_.x, position.x) || BGE::notNearlyEqual(position_.y, position.y)) {
        if (BGE::nearlyZero(position.x)) {
            position_.x = 0.0F;
        } else {
            position_.x = position.x;
        }
        if (BGE::nearlyZero(position.y)) {
            position_.y = 0.0F;
        } else {
            position_.y = position.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setScaleX(float x) {
    if (BGE::notNearlyEqual(scale_.x, x)) {
        if (BGE::nearlyEqual(x, 1.0F)) {
            scale_.x = 1.0F;
        } else {
            scale_.x = x;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setScaleY(float y) {
    if (BGE::notNearlyEqual(scale_.y, y)) {
        if (BGE::nearlyEqual(y, 1.0F)) {
            scale_.y = 1.0F;
        } else {
            scale_.y = y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setScale(Vector2 &scale) {
    if (BGE::notNearlyEqual(scale_.x, scale.x) || BGE::notNearlyEqual(scale_.y, scale.y)) {
        if (BGE::nearlyEqual(scale.x, 1.0F)) {
            scale_.x = 1.0F;
        } else {
            scale_.x = scale.x;
        }
        if (BGE::nearlyEqual(scale.y, 1.0F)) {
            scale_.y = 1.0F;
        } else {
            scale_.y = scale.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setScale(Vector2 &&scale) {
    if (BGE::notNearlyEqual(scale_.x, scale.x) || BGE::notNearlyEqual(scale_.y, scale.y)) {
        if (BGE::nearlyEqual(scale.x, 1.0F)) {
            scale_.x = 1.0F;
        } else {
            scale_.x = scale.x;
        }
        if (BGE::nearlyEqual(scale.y, 1.0F)) {
            scale_.y = 1.0F;
        } else {
            scale_.y = scale.y;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setSkewX(float x) {
    if (BGE::notNearlyEqual(skew_.x, x)) {
        if (BGE::notNearlyZero(x) || BGE::notNearlyZero(skew_.y)) {
            skew_.x = x;
            useSkew_ = true;
        } else {
            skew_.x = 0.0F;
            skew_.y = 0.0F;
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setSkewY(float y) {
    if (BGE::notNearlyEqual(skew_.y, y)) {
        if (BGE::notNearlyZero(skew_.x) || BGE::notNearlyZero(y)) {
            skew_.y = y;
            useSkew_ = true;
        } else {
            skew_.x = 0.0F;
            skew_.y = 0.0F;
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setSkew(Vector2 &skew) {
    if (BGE::notNearlyEqual(skew_.x, skew.x) || BGE::notNearlyEqual(skew_.y, skew.y)) {
        if (BGE::notNearlyZero(skew.x) || BGE::notNearlyZero(skew.y)) {
            skew_.x = skew.x;
            skew_.y = skew.y;
            useSkew_ = true;
        } else {
            skew_.x = 0.0F;
            skew_.y = 0.0F;
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setSkew(Vector2 &&skew) {
    if (BGE::notNearlyEqual(skew_.x, skew.x) || BGE::notNearlyEqual(skew_.y, skew.y)) {
        if (BGE::notNearlyZero(skew.x) || BGE::notNearlyZero(skew.y)) {
            skew_.x = skew.x;
            skew_.y = skew.y;
            useSkew_ = true;
        } else {
            skew_.x = 0.0F;
            skew_.y = 0.0F;
            useSkew_ = false;
        }
        
        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::setRotationInDegrees(float rotation) {
    float radians = rotation * (M_PI/180.0);
    
    setRotationInRadians(radians);
}

void BGE::TransformComponent::setRotationInRadians(float rotation) {
    if (BGE::notNearlyEqual(rotation_, rotation)) {
        if (BGE::nearlyZero(rotation)) {
            rotation_ = 0.0F;
        } else {
            rotation_ = rotation;
        }

        localDirty_ = worldDirty_ = true;
        markChildrenWorldMatrixAsDirty(getSpace());
    }
}

void BGE::TransformComponent::forceDirty() {
    worldDirty_ = true;
    markChildrenWorldMatrixAsDirty(getSpace());
}

void BGE::TransformComponent::updateMatrix() {
    auto space = getSpace();
    updateMatrix(space);
}

void BGE::TransformComponent::updateMatrix(const Space *space) {
    // If our local xform is dirty, compute
    if (localDirty_) {
        Matrix4 mat1;
        Matrix4 mat2;

        if (notNearlyEqual(scale_.x, 1.0f) || notNearlyEqual(scale_.y, 1.0f)) {
            Matrix4MakeScale(mat1, scale_.x, scale_.y, 1.0f);
        } else {
            Matrix4MakeIdentity(mat1);
        }
        
        bool mat2Update = false;
        if (useSkew_) {
            if (notNearlyZero(skew_.x) || notNearlyZero(skew_.y)) {
                Matrix4MakeFlashSkew(mat2, skew_.x, skew_.y);
                mat2Update = true;
            }
        } else {
            if (notNearlyZero(rotation_)) {
                Matrix4MakeRotationZ(mat2, rotation_);
                mat2Update = true;
            }
        }
        
        if (mat2Update) {
            localMatrix_ = mat2 * mat1;
        } else {
            localMatrix_ = std::move(mat1);
        }
        
        if (notNearlyZero(position_.x) || notNearlyZero(position_.y)) {
            Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
            localMatrix_ = mat1 * localMatrix_;
        }
    }
    
    if (worldDirty_ || localDirty_) {
        auto parent = getParent(space);
        
        if (parent) {
            parent->updateMatrix(space);
            worldMatrix_ = parent->worldMatrix_ * localMatrix_;
        } else {
            worldMatrix_ = localMatrix_;
        }
    }
    
    localDirty_ = worldDirty_ = false;
}

void BGE::TransformComponent::updateMatrixLockless(const Space *space) {
    // If our local xform is dirty, compute
    if (localDirty_ ) {
        Matrix4 mat1;
        Matrix4 mat2;

        if (notNearlyEqual(scale_.x, 1.0f) || notNearlyEqual(scale_.y, 1.0f)) {
            Matrix4MakeScale(mat1, scale_.x, scale_.y, 1.0f);
        } else {
            Matrix4MakeIdentity(mat1);
        }
        
        bool mat2Update = false;
        if (useSkew_) {
            if (notNearlyZero(skew_.x) || notNearlyZero(skew_.y)) {
                Matrix4MakeFlashSkew(mat2, skew_.x, skew_.y);
                mat2Update = true;
            }
        } else {
            if (notNearlyZero(rotation_)) {
                Matrix4MakeRotationZ(mat2, rotation_);
                mat2Update = true;
            }
        }
        
        if (mat2Update) {
            localMatrix_ = mat2 * mat1;
        } else {
            localMatrix_ = std::move(mat1);
        }
        
        if (notNearlyZero(position_.x) || notNearlyZero(position_.y)) {
            Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
            localMatrix_ = mat1 * localMatrix_;
        }
    }
    
    if (worldDirty_ || localDirty_) {
        auto parent = getParentLockless(space);
        
        if (parent) {
            parent->updateMatrixLockless(space);
            worldMatrix_ = parent->worldMatrix_ * localMatrix_;
        } else {
            worldMatrix_ = localMatrix_;
        }
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
        
        if (notNearlyEqual(scale_.x, 1.0f) || notNearlyEqual(scale_.y, 1.0f)) {
            Matrix4MakeScale(mat1, scale_.x, scale_.y, 1.0f);
        } else {
            Matrix4MakeIdentity(mat1);
        }
        
        bool mat2Update = false;
        if (useSkew_) {
            if (notNearlyZero(skew_.x) || notNearlyZero(skew_.y)) {
                Matrix4MakeFlashSkew(mat2, skew_.x, skew_.y);
                mat2Update = true;
            }
        } else {
            if (notNearlyZero(rotation_)) {
                Matrix4MakeRotationZ(mat2, rotation_);
                mat2Update = true;
            }
        }
        
        if (mat2Update) {
            localMatrix_ = mat2 * mat1;
        } else {
            localMatrix_ = std::move(mat1);
        }
        
        if (notNearlyZero(position_.x) || notNearlyZero(position_.y)) {
            Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
            localMatrix_ = mat1 * localMatrix_;
        }
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
    
    Matrix4MultiplyVector2(results, mtx, Vector2{{0, 0}});
                           
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

void BGE::TransformComponent::setLocalMatrix(const Matrix4& matrix) {
    localMatrix_ = matrix;
    localDirty_ = false;
    worldDirty_ = true;
    markChildrenWorldMatrixAsDirty(getSpace());
}

BGE::TransformComponent *BGE::TransformComponent::getParent() const {
    auto space = getSpace();
    
    return space->getComponent<TransformComponent>(parentHandle_.getHandle());
}

BGE::TransformComponent *BGE::TransformComponent::getParent(const Space *space) const {
    return space->getComponent<TransformComponent>(parentHandle_.getHandle());
}

BGE::TransformComponent *BGE::TransformComponent::getParentLockless(const Space *space) const {
    return space->getComponentLockless<TransformComponent>(parentHandle_.getHandle());
}

std::vector<BGE::TransformComponentHandle> BGE::TransformComponent::getOrderedChildrenHandles() {
    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(getSpace());
    }
    return sortedChildrenHandles_;
}

void BGE::TransformComponent::getOrderedChildrenHandles(std::vector<BGE::TransformComponentHandle>& children) {
    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(getSpace());
    }

    children.clear();
    children.reserve(sortedChildrenHandles_.size());
    children.assign(sortedChildrenHandles_.begin(), sortedChildrenHandles_.end());
}

std::vector<BGE::TransformComponentHandle> BGE::TransformComponent::getReverseOrderedChildrenHandles() {
    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(getSpace());
    }

    return std::vector<BGE::TransformComponentHandle>(sortedChildrenHandles_.rbegin(), sortedChildrenHandles_.rend());
}

void BGE::TransformComponent::getReverseOrderedChildrenHandles(std::vector<BGE::TransformComponentHandle>& children) {
    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(getSpace());
    }
    
    children.clear();
    children.reserve(sortedChildrenHandles_.size());
    children.assign(sortedChildrenHandles_.rbegin(), sortedChildrenHandles_.rend());
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

void BGE::TransformComponent::getChildren(Space *space, std::vector<BGE::TransformComponent *>& children) {
    children.clear();
    children.reserve(childrenHandles_.size());
    
    for (auto handle : childrenHandles_) {
        auto xform = space->getComponent<TransformComponent>(handle.getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
}

void BGE::TransformComponent::sortChildren(Space *space) {
    auto size = childrenHandles_.size();
    if (size <= 1) {
        if (size == 1) {
            sortedChildrenHandles_ = childrenHandles_;
        }
        return;
    }
    std::vector<TransformComponent *> children;
    children.reserve(childrenHandles_.size());
    for (auto h : childrenHandles_) {
        auto xform = space->getComponent<TransformComponent>(h);
        if (xform) {
            children.push_back(xform);
        }
    }
    
    TransformComponent **xforms = &children[0];
    size_t i, j;
    
    size = children.size();
    for (i=1;i<size;++i) {
        j = i;
        
        while (j > 0 && xforms[j]->getZ() < xforms[j-1]->getZ()) {
            auto temp = xforms[j];
            xforms[j] = xforms[j-1];
            xforms[j-1] = temp;
            --j;
        }
    }
    sortedChildrenHandles_.clear();
    for (auto c : children) {
        sortedChildrenHandles_.push_back(c->getHandle<TransformComponent>());
    }
}

std::vector<BGE::TransformComponent *> BGE::TransformComponent::getOrderedChildren() {
    handleServicesLock();

    auto space = getSpace();

    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(space);
    }

    std::vector<TransformComponent *> children;
    children.reserve(sortedChildrenHandles_.size());
    for (auto handle : sortedChildrenHandles_) {
        auto xform = space->getComponentLockless<TransformComponent>(handle.getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    handleServicesUnlock();

    return children;
}

void BGE::TransformComponent::getOrderedChildren(Space *space, std::vector<BGE::TransformComponent *>& children) {
    handleServicesLock();
    
    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(space);
    }
    
    children.clear();
    children.reserve(sortedChildrenHandles_.size());
    for (auto handle : sortedChildrenHandles_) {
        auto xform = space->getComponentLockless<TransformComponent>(handle.getHandle());
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    handleServicesUnlock();
}

std::vector<BGE::TransformComponent *> BGE::TransformComponent::getReverseOrderedChildren() {
    handleServicesLock();
    
    auto space = getSpace();

    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(space);
    }

    std::vector<TransformComponent *> children;
    children.reserve(sortedChildrenHandles_.size());

    for (auto it=sortedChildrenHandles_.rbegin();it!=sortedChildrenHandles_.rend();++it) {
        auto xform = space->getComponentLockless<TransformComponent>(*it);
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    handleServicesUnlock();
    
    return children;
}

void BGE::TransformComponent::getReverseOrderedChildren(Space *space, std::vector<BGE::TransformComponent *>& children) {
    handleServicesLock();

    if (childrenHandles_.size() != sortedChildrenHandles_.size()) {
        sortChildren(space);
    }

    children.reserve(sortedChildrenHandles_.size());
    
    for (auto it=sortedChildrenHandles_.rbegin();it!=sortedChildrenHandles_.rend();++it) {
        auto xform = space->getComponentLockless<TransformComponent>(*it);
        
        if (xform) {
            children.push_back(xform);
        }
    }
    
    handleServicesUnlock();
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
    sortedChildrenHandles_.clear();
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

BGE::TransformComponent *BGE::TransformComponent::childAtIndex(const Space *space, uint32_t index) {
    if (index < childrenHandles_.size()) {
        auto handle = childrenHandles_.at(index);
        return space->getComponent<TransformComponent>(handle.getHandle());
    } else {
        return nullptr;
    }
}

BGE::TransformComponent *BGE::TransformComponent::childAtIndexLockless(const Space *space, uint32_t index) {
    if (index < childrenHandles_.size()) {
        auto handle = childrenHandles_.at(index);
        return space->getComponentLockless<TransformComponent>(handle.getHandle());
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
