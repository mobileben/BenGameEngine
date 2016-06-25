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
    class Space;
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
        
        bool hasGameObject() const { return !gameObject_.expired(); }
        std::weak_ptr<GameObject> getGameObject() const { return gameObject_; }
        std::weak_ptr<Space> getSpace() const { return space_; }
        
    protected:
        Component() = delete;
        Component(Component const&) = delete;
        Component(uint64_t componentId);
        Component(uint64_t componentId, std::string name);

        void setGameObject(std::shared_ptr<GameObject> gameObject) { gameObject_ = gameObject; }
        void setSpace(std::shared_ptr<Space> space) { space_ = space; }

        virtual void created() {}
        
    private:
        friend ComponentService;
        friend GameObject;
        friend Space;
        
        std::weak_ptr<GameObject>   gameObject_;
        std::weak_ptr<Space>        space_;
        
    };
}

#endif /* BGEComponent_h */
