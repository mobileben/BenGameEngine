//
//  Object.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Object.h"


BGE::Object::Object() : id_(0) {
}

BGE::Object::Object(Object const& object) {
    if (&object != this) {
        *this = object;
    }
}

BGE::Object::Object(uint64_t objId) : id_(objId) {
}


BGE::Object::Object(uint64_t objId, std::string name) : id_(objId), name_(name) {
}


BGE::Object::Object(uint64_t objId, std::string name, std::string domain) : id_(objId), name_(name), domain_(domain) {
}

BGE::Object& BGE::Object::operator=(BGE::Object const& b) {
    
    id_ = b.id_;
    name_ = b.name_;
    domain_ = b.domain_;
    
    return *this;
}