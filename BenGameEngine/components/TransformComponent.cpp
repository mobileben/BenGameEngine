//
//  TransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TransformComponent.h"
#include "ComponentBitmask.h"
#include <cassert>

uint32_t BGE::TransformComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::TransformComponent::type_index_ = typeid(BGE::TransformComponent);

std::shared_ptr<BGE::TransformComponent> BGE::TransformComponent::create(ObjectId componentId) {
    return std::make_shared<TransformComponent>(private_key{}, componentId);
}

BGE::TransformComponent::TransformComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), visible_(true),
bounds_({ 0, 0, 0, 0}), position_({ 0, 0 }), z_(0), scale_( { 1, 1 }), skew_({ 0, 0 }), rotation_(0), transformDirty_(false), speed_(1), paused_(false) {
    Matrix4MakeIdentify(matrix_);
}

void BGE::TransformComponent::setGameObject(std::shared_ptr<GameObject> gameObject) {
    Component::setGameObject(gameObject);
}

void BGE::TransformComponent::updateMatrix() {
    Matrix4 mat1;
    Matrix4 mat2;
    
    Matrix4MakeScale(mat1, scale_.x, scale_.y, 1);
    Matrix4MakeRotationZ(mat2, rotation_);
    
    localMatrix_ = mat2 * mat1;
    Matrix4MakeTranslation(mat1, position_.x, position_.y, 0);
    localMatrix_ = mat1 * localMatrix_;
    
    auto parent = getParent().lock();
    
    if (parent) {
        parent->getMatrix(mat1);
        
        matrix_ = mat1 * localMatrix_;
    } else {
        matrix_ = localMatrix_;
    }
}

void BGE::TransformComponent::getMatrix(Matrix4 &matrix) {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    matrix = matrix_;
}

const float *BGE::TransformComponent::getMatrixRaw() {
    if (transformDirty_) {
        updateMatrix();
        
        transformDirty_ = false;
    }
    
    return matrix_.m;
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

void BGE::TransformComponent::addChild(std::shared_ptr<TransformComponent> child) {
    assert(child);
    assert(child->getParent().expired());
    
    if (child) {
        children_.push_back(child);
        child->parent_ = derived_shared_from_this<TransformComponent>();
        
        // TODO: Update hierarchy
    }
}

void BGE::TransformComponent::removeAllChildren() {
    for (auto child : children_) {
        // Remove parent from all children
        child->parent_.reset();
    }
    
    children_.clear();
}

void BGE::TransformComponent::removeFromParent() {
    if (!parent_.expired()) {
        std::shared_ptr<TransformComponent> parent = parent_.lock();
        std::vector<std::shared_ptr<TransformComponent>>::iterator it;
        
        it = std::find(parent->children_.begin(), parent->children_.end(), derived_shared_from_this<TransformComponent>());
        
        if (it != parent->children_.end()) {
            parent->children_.erase(it);
        }
        
        parent_.reset();
    } else {
        parent_.reset();
    }
}

void BGE::TransformComponent::insertChild(std::shared_ptr<TransformComponent> child, uint32_t index) {
    assert(child);
    assert(child->getParent().expired());

    if (child) {
        if (index < children_.size()) {
            children_.insert(children_.begin() + index, child);
        } else {
            children_.push_back(child);
        }
        
        child->parent_ = derived_shared_from_this<TransformComponent>();
    }
}

void BGE::TransformComponent::moveToParent(std::shared_ptr<TransformComponent> parent) {
    assert(parent);
    
    if (parent) {
        removeFromParent();
        
        parent->addChild(derived_shared_from_this<TransformComponent>());
    }
}

std::shared_ptr<BGE::TransformComponent> BGE::TransformComponent::childAtIndex(uint32_t index) {
    if (index < children_.size()) {
        return children_.at(index);
    } else {
        return nullptr;
    }
}

bool BGE::TransformComponent::hasChild(std::shared_ptr<TransformComponent> child, bool descend) {
    assert(child);
    
    if (child) {
        if (descend) {
            for (auto transform : children_) {
                if (transform == child) {
                    return true;
                } else {
                    bool found = transform->hasChild(child, descend);
                    
                    if (found) {
                        return found;
                    }
                }
            }
            
            return false;
        } else {
            return std::find(children_.begin(), children_.end(), child) != children_.end();
        }
    } else {
        return false;
    }
}

bool BGE::TransformComponent::inParentHierarchy(std::shared_ptr<TransformComponent> parent) {
    assert(parent);
    
    if (parent) {
        return parent->hasChild(derived_shared_from_this<TransformComponent>(), true);
    } else {
        return false;
    }
}
