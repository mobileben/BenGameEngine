//
//  GameObject.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GameObject.h"
#include "Space.h"

std::shared_ptr<BGE::GameObject> BGE::GameObject::create(uint64_t objId) {
    return std::make_shared<GameObject>(private_key{}, objId);
}

std::shared_ptr<BGE::GameObject> BGE::GameObject::create(uint64_t objId, std::string name) {
    return std::make_shared<GameObject>(private_key{}, objId, name);
}

BGE::GameObject::GameObject(struct private_key const&, uint64_t objId) : BGE::Object(objId), active_(true) {
}

BGE::GameObject::GameObject(struct private_key const&, uint64_t objId, std::string name) : BGE::Object(objId, name), active_(true) {
}

BGE::GameObject::~GameObject() {
    removeAllComponents();
}

void BGE::GameObject::removeAllComponents() {
    auto space = getSpace().lock();
    
    if (space) {
        for (auto component : components_) {
            const std::type_info& typeId = typeid(component.second);
            space->removeComponent(std::type_index(typeId), component.second->getInstanceId());
        }
    }
    
    components_.clear();
}