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
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<Component> create(uint64_t componentId);
        static std::shared_ptr<Component> create(uint64_t componentId, std::string name);
        
        Component(struct private_key const& key, uint64_t componentId);
        Component(struct private_key const& key, uint64_t componentId, std::string name);
        virtual ~Component() {}

    protected:
        Component() = delete;
        Component(Component const&) = delete;
        Component(uint64_t componentId);
        Component(uint64_t componentId, std::string name);
        
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
