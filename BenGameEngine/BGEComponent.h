//
//  BGEComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponent_h
#define BGEComponent_h

#include <stdio.h>
#include "BGEObject.h"

class BGEGameObject;

class BGEComponent : public BGEObject
{
public:
    BGEComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject);
    
    BGEComponent(uint64_t componentId) = delete;
    BGEComponent(uint64_t componentId, std::string name) = delete;
    BGEComponent() = delete;
    BGEComponent(BGEComponent const&) = delete;
    virtual ~BGEComponent() {}
    
private:
    std::weak_ptr<BGEGameObject> gameObject_;
    
};

#endif /* BGEComponent_h */
