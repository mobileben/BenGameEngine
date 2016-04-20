//
//  Component.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Component.h"

BGE::Component::Component(uint64_t componentId) : BGE::Object(componentId) {
}

BGE::Component::Component(uint64_t componentId, std::string name) : BGE::Object(componentId, name) {
}

BGE::Component::Component(uint64_t componentId, std::string name, std::shared_ptr<GameObject> gameObject) : BGE::Object(componentId, name), gameObject_(gameObject) {
}
