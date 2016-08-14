//
//  NamedObject.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef NamedObject_h
#define NamedObject_h

#include <stdio.h>
#include "Object.h"
#include <string>

namespace BGE {
    class NamedObject : public Object
    {
    public:
        NamedObject() : Object() {}
        NamedObject(ObjectId objId) : Object(objId) {}
        NamedObject(ObjectId objId, std::string name) : Object(objId), name_(name) {}
        virtual ~NamedObject() {}
        
        void setName(std::string name) { name_ = name; }
        std::string getName() const { return name_; }
        
    private:
        std::string name_;
    };
}
#endif /* NamedObject_h */
