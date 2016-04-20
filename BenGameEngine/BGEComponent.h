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
class BGEComponentService;

class BGEComponent : public BGEObject
{
public:
    BGEComponent() = delete;
    BGEComponent(BGEComponent const&) = delete;
    virtual ~BGEComponent() {}
    
protected:
    BGEComponent(uint64_t componentId);
    BGEComponent(uint64_t componentId, std::string name);
    BGEComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject);

    bool hasGameComponent() const { return !gameObject_.expired(); }
    std::weak_ptr<BGEGameObject> getGameObject() const { return gameObject_; }
    void setGameObject(std::shared_ptr<BGEGameObject> gameObject) { gameObject_ = gameObject; }
    
private:
    friend BGEComponentService;
    friend BGEGameObject;
    
    std::weak_ptr<BGEGameObject> gameObject_;
    
};

#endif /* BGEComponent_h */
