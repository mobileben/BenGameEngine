//
//  BGEComponentService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEComponentService.h"

BGEComponentService::BGEComponentService() {
}

BGEComponentService::~BGEComponentService() {
}

template <typename T>
std::shared_ptr<T> BGEComponentService::getComponent(uint64_t componentId) {
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
std::shared_ptr<T> BGEComponentService::getComponent(std::string name) {
    std::type_index index(typeid(T));
    ComponentMapIterator it = components_.find(index);
    
    if (it != components_.end()) {
        for (auto component : it->second) {
            if (component->getName() == name) {
                return component;
            }
        }
    }
    
    return nullptr;
}

template <typename T>
void BGEComponentService::addComponent(std::shared_ptr<T> component) {
    std::type_index index(typeid(T));
    ComponentMapIterator it = components_.find(index);
    
    if (it != components_.end()) {
        // We already have a vector of type T
    } else {
        ComponentVector components(component);
        components_[index] = components;
    }
}

template <typename T>
void BGEComponentService::removeComponent(uint64_t componentId) {
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
void BGEComponentService::removeComponent(std::string name) {
    std::type_index index(typeid(T));
    ComponentMapIterator it = components_.find(index);
    
    if (it != components_.end()) {
        ComponentVector vec = it->second;
        
        for (ComponentVectorIterator vit=vec.begin();vit != vec.end();vit++) {
            if ((*vit)->getName() == name) {
                vec.erase(vit);
                return;
            }
        }
    }
}

template <typename T>
void BGEComponentService::removeAllComponents() {
    components_.erase(typeid(T));
}