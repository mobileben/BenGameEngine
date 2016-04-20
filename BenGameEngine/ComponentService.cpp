//
//  ComponentService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ComponentService.h"

BGE::ComponentService::ComponentService() {
}

BGE::ComponentService::~ComponentService() {
}

template <typename T>
std::shared_ptr<T> BGE::ComponentService::getComponent(uint64_t componentId) {
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
std::shared_ptr<T> BGE::ComponentService::getComponent(std::string name) {
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
void BGE::ComponentService::removeComponent(uint64_t componentId) {
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
void BGE::ComponentService::removeComponent(std::string name) {
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
void BGE::ComponentService::removeAllComponents() {
    components_.erase(typeid(T));
}