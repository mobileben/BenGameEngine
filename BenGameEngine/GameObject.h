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
#include <cassert>
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
        
        ~GameObject();
        
        template <typename T> std::shared_ptr<T> getComponent() {
            std::type_index index(typeid(T));
            
            if(components_.count(index) != 0) {
                return std::static_pointer_cast<T>(components_[index]);
            } else {
                return nullptr;
            }
        }
        
        template <typename T> void addComponent(std::shared_ptr<T> component) {
            assert(!component->hasGameObject());
            components_[typeid(T)] = component;
            component->setGameObject(derived_shared_from_this<GameObject>());
        }
        
        template <typename T>
        void removeComponent() {
            auto space = getSpace().lock();
            std::type_index typeId = typeid(T);
            
            if (space) {
                auto component = components_.find(typeId);
                
                if (component != components_.end()) {
#ifdef NOT_YET
                    space->removeComponent<T>(component->second->getInstanceId());
#endif
                }
            }
            
            components_.erase(typeId);
        }
        
        void removeAllComponents();
        
        bool isActive() const { return active_; }
        void setActive(bool active) { active_ = active; }

        std::weak_ptr<Space> getSpace() const { return space_; }

    protected:
        
        GameObject() = delete;
        GameObject(GameObject const&) = delete;
        GameObject(uint64_t objId) = delete;
        GameObject(uint64_t objId, std::string name) = delete;

        void setSpace(std::shared_ptr<Space> space) { space_ = space; }

    private:
        friend GameObjectService;
        
        bool                    active_;
        std::shared_ptr<Space>  space_;
        std::unordered_map<std::type_index, std::shared_ptr<BGE::Component>> components_;
    };
}

#endif /* GameObject_hpp */
