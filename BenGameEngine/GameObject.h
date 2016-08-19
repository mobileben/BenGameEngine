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
    public:
        GameObject();
        GameObject(ObjectId objId);
        ~GameObject();
        
        void initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, std::string name);
        
        inline GameObjectHandle getHandle() const { return handle_; }
        
        inline bool isActive() const { return active_; }
        inline void setActive(bool active) { active_ = active; }

        Space *getSpace() const;
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        
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
            component->setGameObjectHandle(getHandle());
        }
        
        template <typename T> void addComponent(T *component) {
            assert(!component->hasGameObject());
            ComponentHandle handle{T::bitmask_, component->getRawHandle()};
            componentBitmask_ |= T::bitmask_;
            componentsNew_.push_back(handle);
            component->setGameObjectHandle(getHandle());
        }
        
        template <typename T>
        void removeComponent() {
            if (hasComponent<T>()) {
                std::type_index typeId = Component::getTypeIndex<T>();
                
                removeComponentFromSpace(typeId);
                
//                componentBitmask_ &= ~ComponentBitmask::bitmaskForTypeIndex(typeId);
                componentBitmask_ &= ~Component::getBitmask<T>();
                
                components_.erase(typeId);
            }
        }
        
        void removeAllComponents();
        
        inline uint32_t getComponentBitmask() const { return componentBitmask_; }
        
        template <typename T>
        inline bool hasComponent() const {
            return getComponentBitmask() & Component::getBitmask<T>();
        }
        
        inline bool operator==(const GameObject &other) const {
            return getInstanceId() == other.getInstanceId();
        }
        
        inline bool operator!=(const GameObject &other) const {
            return getInstanceId() != other.getInstanceId();
        }
        
    private:
        friend GameObjectService;
        
        bool                    active_;
        uint32_t                componentBitmask_;
        GameObjectHandle        handle_;
        SpaceHandle             spaceHandle_;
        std::unordered_map<std::type_index, std::shared_ptr<Component>> components_;
        std::vector<ComponentHandle>    componentsNew_;
        
        void removeComponentFromSpace(std::type_index typeIndex);
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
    };
}

#endif /* GameObject_hpp */
