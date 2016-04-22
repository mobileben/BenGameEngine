//
//  GameObject.hpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <stdio.h>
#include "Object.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Component.h"

namespace BGE {
    class Space;
    class GameObjectService;
    
    class GameObject : public BGE::Object
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<GameObject> create(uint64_t objId);
        static std::shared_ptr<GameObject> create(uint64_t objId, std::string name);

        GameObject(struct private_key const&, uint64_t objId);
        GameObject(struct private_key const&, uint64_t objId, std::string name);
        
        virtual ~GameObject() {}
        
        template <typename T> std::shared_ptr<T> getComponent() {
            std::type_index index(typeid(T));
            
            if(components_.count(index) != 0) {
                return std::static_pointer_cast<T>(components_[index]);
            } else {
                return nullptr;
            }
        }
        
        template <typename T> void addComponent(std::shared_ptr<T> component) {
            assert(!component->hasGameComponent());
            components_[typeid(T)] = component;
            component->setGameObject(derived_shared_from_this<GameObject>());
        }
        
        template <typename T> void removeComponent();
        void removeAllComponents();
        
        bool isActive() const { return active_; }
        void setActive(bool active) { active_ = active; }

    protected:
        
        GameObject() = delete;
        GameObject(GameObject const&) = delete;
        GameObject(uint64_t objId) = delete;
        GameObject(uint64_t objId, std::string name) = delete;

    private:
        friend GameObjectService;
        
        bool active_;
        std::unordered_map<std::type_index, std::shared_ptr<BGE::Component>> components_;
    };
}

#endif /* GameObject_hpp */
