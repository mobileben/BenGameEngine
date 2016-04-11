//
//  BGERenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGERenderComponent.h"

BGERenderComponent::BGERenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject) : BGEComponent(componentId, name, gameObject), bounds_({0, 0, 0, 0}), enabled_(true) {
    
}