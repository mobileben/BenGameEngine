//
//  BGETransformComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETransformComponent.h"
#include <cassert>

BGETransformComponent::BGETransformComponent(uint64_t componentId) :  Component(componentId), visible_(true), interactable_(true), interactableWhenHidden_(false),
                                                    bounds_({ 0, 0, 0, 0}), position_({ 0, 0 }),
                                                    z_(0), scale_( { 1, 1 }), skew_({ 0, 0 }), rotation_(0),
                                                    transformDirty_(false), speed_(1), paused_(false) {
    BGEMatrix4MakeIdentify(matrix_);
}

BGETransformComponent::BGETransformComponent(uint64_t componentId, std::string name) :  Component(componentId, name), visible_(true), interactable_(true), interactableWhenHidden_(false),
bounds_({ 0, 0, 0, 0}), position_({ 0, 0 }),
z_(0), scale_( { 1, 1 }), skew_({ 0, 0 }), rotation_(0),
transformDirty_(false), speed_(1), paused_(false) {
    BGEMatrix4MakeIdentify(matrix_);
}

void BGETransformComponent::getMatrix(BGEMatrix4 &matrix) {
    if (transformDirty_) {
        
    } else {
        matrix = matrix_;
    }
}

void BGETransformComponent::addChild(std::shared_ptr<BGETransformComponent> child) {
    assert(child);
    assert(child->getParent().expired());
    
    if (child) {
        children_.push_back(child);
        child->parent_ = derived_shared_from_this<BGETransformComponent>();
        
        // TODO: Update hierarchy
    }
}

void BGETransformComponent::removeAllChildren() {
    for (auto child : children_) {
        // Remove parent from all children
        child->parent_.reset();
    }
    
    children_.clear();
}

void BGETransformComponent::removeFromParent() {
    if (!parent_.expired()) {
        std::shared_ptr<BGETransformComponent> parent = parent_.lock();
        std::vector<std::shared_ptr<BGETransformComponent>>::iterator it;
        
        it = std::find(parent->children_.begin(), parent->children_.end(), derived_shared_from_this<BGETransformComponent>());
        
        if (it != parent->children_.end()) {
            parent->children_.erase(it);
        }
        
        parent_.reset();
    } else {
        parent_.reset();
    }
}

void BGETransformComponent::insertChild(std::shared_ptr<BGETransformComponent> child, uint32_t index) {
    assert(child);
    assert(child->getParent().expired());

    if (child) {
        if (index < children_.size()) {
            children_.insert(children_.begin() + index, child);
        } else {
            children_.push_back(child);
        }
        
        child->parent_ = derived_shared_from_this<BGETransformComponent>();
    }
}

void BGETransformComponent::moveToParent(std::shared_ptr<BGETransformComponent> parent) {
    assert(parent);
    
    if (parent) {
        removeFromParent();
        
        parent->addChild(derived_shared_from_this<BGETransformComponent>());
    }
}

std::shared_ptr<BGETransformComponent> BGETransformComponent::childAtIndex(uint32_t index) {
    if (index < children_.size()) {
        return children_.at(index);
    } else {
        return nullptr;
    }
}

std::shared_ptr<BGETransformComponent> BGETransformComponent::childWithName(std::string name, bool descend) {
    assert(name != "");
    
    for (auto transform : children_) {
        if (transform->getName() == name) {
            return transform;
        } else if (descend) {
            auto foundtransform = transform->childWithName(name, descend);
            
            if (foundtransform) {
                return foundtransform;
            }
        }
    }
    
    return nullptr;
}

bool BGETransformComponent::hasChild(std::shared_ptr<BGETransformComponent> child, bool descend) {
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

bool BGETransformComponent::inParentHierarchy(std::shared_ptr<BGETransformComponent> parent) {
    assert(parent);
    
    if (parent) {
        return parent->hasChild(derived_shared_from_this<BGETransformComponent>(), true);
    } else {
        return false;
    }
}
