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

BGE::GameObject::GameObject() : NamedObject(), active_(false), componentBitmask_(0) {
}

BGE::GameObject::GameObject(ObjectId objId) : NamedObject(objId), active_(false), componentBitmask_(0) {
}

BGE::GameObject::~GameObject() {
    removeAllComponents();
}

void BGE::GameObject::initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, std::string name) {
    setName(name);

    active_ = false;

    removeAllComponents();

    handle_ = gameObjHandle;
    spaceHandle_ = spaceHandle;
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
