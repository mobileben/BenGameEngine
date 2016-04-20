//
//  GameObject.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GameObject.h"

BGE::GameObject::GameObject(uint64_t objId) : BGE::Object(objId), active_(true) {
}

BGE::GameObject::GameObject(uint64_t objId, std::string name) : BGE::Object(objId, name), active_(true) {
}

template <typename T>
void BGE::GameObject::removeComponent() {
    components_.erase(typeid(T));
}

void BGE::GameObject::removeAllComponents() {
    components_.clear();
}