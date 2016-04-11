//
//  BGEGameObject.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEGameObject.h"

BGEGameObject::BGEGameObject(uint64_t objId) : BGEObject(objId), active_(true) {
}

BGEGameObject::BGEGameObject(uint64_t objId, std::string name) : BGEObject(objId, name), active_(true) {
}

template <typename T>
std::shared_ptr<T> BGEGameObject::getComponent() {
    std::type_index index(typeid(T));
    
    if(components_.count(index) != 0) {
        return std::static_pointer_cast<T>(components_[index]);
    } else {
        return nullptr;
    }
}

template <typename T>
void BGEGameObject::addComponent(std::shared_ptr<T> component) {
    components_[typeid(T)] = component;
}

template <typename T>
void BGEGameObject::removeComponent() {
    components_.erase(typeid(T));
}

void BGEGameObject::removeAllComponents() {
    components_.clear();
}