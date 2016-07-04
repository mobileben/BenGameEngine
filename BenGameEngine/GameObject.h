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
#include "NamedObject.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Component.h"

namespace BGE {
    class Space;
    class GameObjectService;
    
    class GameObject : public NamedObject
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<GameObject> create(ObjectId objId);
        static std::shared_ptr<GameObject> create(ObjectId objId, std::string name);

        GameObject(struct private_key const&, ObjectId objId);
        GameObject(struct private_key const&, ObjectId objId, std::string name);
        
        ~GameObject();
        
        template <typename T> std::shared_ptr<T> getComponent() {
            std::type_index index = Component::getTypeIndex<T>();
            
#if DEBUG
            if(components_.count(index) != 0) {
                return std::static_pointer_cast<T>(components_[index]);
            } else {
                return nullptr;
            }
#else
            return std::static_pointer_cast<T>(components_[index]);
#endif
        }
        
        template <typename T> void addComponent(std::shared_ptr<T> component) {
            assert(!component->hasGameObject());
            componentBitmask_ |= Component::getBitmask<T>();
            components_[typeid(T)] = component;
            component->setGameObject(derived_shared_from_this<GameObject>());
        }
        
        template <typename T>
        void removeComponent() {
            std::type_index typeId = typeid(T);
            
            removeComponent(typeId);
        }
        
        void removeComponent(std::type_index typeIndex);
        
        void removeAllComponents();
        
        bool isActive() const { return active_; }
        void setActive(bool active) { active_ = active; }

        inline uint32_t getComponentBitmask() const { return componentBitmask_; }
        
        template <typename T>
        inline bool hasComponent() const {
            return getComponentBitmask() & Component::getBitmask<T>();
        }
        
        Space *getSpace() const;
        SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        
    protected:
        
#ifdef NOT_YET
        GameObject() = delete;
        GameObject(GameObject const&) = delete;
        GameObject(ObjectId objId) = delete;
        GameObject(ObjectId objId, std::string name) = delete;
#endif
        
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }

    private:
        friend GameObjectService;
        
        bool                    active_;
        uint32_t                componentBitmask_;
        SpaceHandle             spaceHandle_;
        std::unordered_map<std::type_index, std::shared_ptr<Component>> components_;
    };
}

#endif /* GameObject_hpp */
