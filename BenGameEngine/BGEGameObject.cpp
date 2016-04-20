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
void BGEGameObject::removeComponent() {
    components_.erase(typeid(T));
}

void BGEGameObject::removeAllComponents() {
    components_.clear();
}