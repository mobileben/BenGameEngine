//
//  BGEComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEComponent.h"

BGEComponent::BGEComponent(uint64_t componentId) : BGEObject(componentId) {
}

BGEComponent::BGEComponent(uint64_t componentId, std::string name) : BGEObject(componentId, name) {
}

BGEComponent::BGEComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject) : BGEObject(componentId, name), gameObject_(gameObject) {
}
