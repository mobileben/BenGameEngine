//
//  Component.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponent_h
#define BGEComponent_h

#include <stdio.h>
#include "Object.h"

namespace BGE {
    class GameObject;
    class ComponentService;
    
    class Component : public BGE::Object
    {
    public:
        Component() = delete;
        Component(Component const&) = delete;
        virtual ~Component() {}
        
    protected:
        Component(uint64_t componentId);
        Component(uint64_t componentId, std::string name);
        Component(uint64_t componentId, std::string name, std::shared_ptr<GameObject> gameObject);
        
        bool hasGameComponent() const { return !gameObject_.expired(); }
        std::weak_ptr<GameObject> getGameObject() const { return gameObject_; }
        void setGameObject(std::shared_ptr<GameObject> gameObject) { gameObject_ = gameObject; }
        
    private:
        friend ComponentService;
        friend GameObject;
        
        std::weak_ptr<GameObject> gameObject_;
        
    };
}

#endif /* BGEComponent_h */
