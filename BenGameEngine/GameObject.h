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
#include "Component.h"
#include "Space.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace BGE {
    class Space;
    class GameObjectService;
    
    class GameObject : public NamedObject
    {
    public:
        GameObject();
        GameObject(ObjectId objId);
        ~GameObject() {}
        
        void initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, std::string name);
        void destroy();
        
        inline GameObjectHandle getHandle() const { return handle_; }
        
        inline bool isActive() const { return active_; }
        inline void setActive(bool active) { active_ = active; }

        Space *getSpace() const;
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        
        template <typename T> void addComponent(T *component) {
            assert(!component->hasGameObject());
            auto bitmask = T::bitmask_;
            ComponentHandle handle{T::typeId_, component->getRawHandle()};
            componentBitmask_ |= bitmask;
            components_.push_back(handle);
            
            component->setGameObjectHandle(getHandle());
        }
        
        template <typename T> T *getComponent() {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto const &handle : components_) {
                    if (handle.typeId == typeId) {
                        auto space = getSpace();
                        
                        return space->getComponent<T>(handle);
                    }
                }
            }
            
            return nullptr;
        }
        
        template <typename T> void removeComponent() {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto it = components_.begin();it != components_.end();++it) {
                    if (it->typeId == typeId) {
                        auto space = getSpace();
                        
                        space->removeComponent(*it);
                        componentBitmask_ &= ~T::bitmask_;
                        
                        components_.erase(it);
                        return;
                    }
                }
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
        
        bool                            active_;
        uint32_t                        componentBitmask_;
        GameObjectHandle                handle_;
        SpaceHandle                     spaceHandle_;
        std::vector<ComponentHandle>    components_;
        
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
    };
}

#endif /* GameObject_hpp */
