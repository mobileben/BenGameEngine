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

std::shared_ptr<BGE::GameObject> BGE::GameObject::create(ObjectId objId) {
    return std::make_shared<GameObject>(private_key{}, objId);
}

std::shared_ptr<BGE::GameObject> BGE::GameObject::create(ObjectId objId, std::string name) {
    return std::make_shared<GameObject>(private_key{}, objId, name);
}

BGE::GameObject::GameObject(struct private_key const&, ObjectId objId) : NamedObject(objId), active_(true), componentBitmask_(0) {
}

BGE::GameObject::GameObject(struct private_key const&, ObjectId objId, std::string name) : NamedObject(objId, name), active_(true), componentBitmask_(0) {
}

BGE::GameObject::~GameObject() {
    removeAllComponents();
}

void BGE::GameObject::removeComponentFromSpace(std::type_index typeIndex) {
    auto space = getSpace();
    
    if (space) {
#if DEBUG
        auto component = components_.find(typeIndex);
      
        assert(component != components_.end());
        
        if (component != components_.end()) {
            space->removeComponent(typeIndex, component->second->getInstanceId());
        }
#else
        auto component = components_[typeIndex];
        space->removeComponent(typeIndex, component->second->getInstanceId());
#endif
    }
}

void BGE::GameObject::removeAllComponents() {
    auto space = getSpace();
    
    if (space) {
        for (auto component : components_) {
            const std::type_info& typeId = typeid(component.second);
            removeComponentFromSpace(typeId);
        }
    }
    
    componentBitmask_ = 0;
    components_.clear();
}

BGE::Space *BGE::GameObject::getSpace() const {
    return Game::getInstance()->getSpaceService()->getSpace(spaceHandle_);
}
