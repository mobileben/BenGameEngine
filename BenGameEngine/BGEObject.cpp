//
//  BGEObject.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEObject.h"

BGEObject::BGEObject(uint64_t objId) : id_(objId) {
}


BGEObject::BGEObject(uint64_t objId, std::string name) : id_(objId), name_(name) {
}


BGEObject::BGEObject(uint64_t objId, std::string name, std::string domain) : id_(objId), name_(name), domain_(domain) {
}