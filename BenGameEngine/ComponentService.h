//
//  ComponentService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponentService_h
#define BGEComponentService_h

#include <stdio.h>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <type_traits>

#include "Service.h"
#include "Component.h"

namespace BGE {
    class Space;
    
    class ComponentService : public BGE::Service
    {
    public:
        ComponentService();
        ComponentService(SpaceHandle spaceHandle);
        ~ComponentService();
        
        static void registerComponents();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}

        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        SpaceHandle getSpaceHandle(void) const { return spaceHandle_; }
        
        template <typename T, typename... Args> std::shared_ptr<T> createComponent(Args&& ...args) {
            static_assert(std::is_base_of<Component, T>::value, "Not Component");
            ObjectId objId = getIdAndIncrement();
            std::shared_ptr<T> component = T::create(objId, std::forward<Args>(args)...);
            
            component->setSpaceHandle(spaceHandle_);
            addComponent(component);
            
            component->created();
            
            return component;
        }
        
        template <typename T> std::shared_ptr<T> getComponent(ObjectId componentId);
        
        template <typename T> void removeComponent(ObjectId componentId);
        template <typename T> void removeAllComponents();
        
        void removeComponent(std::type_index typeIndex, ObjectId componentId);
        
    private:
        typedef std::vector<std::shared_ptr<Component>> ComponentVector;
        typedef std::vector<std::shared_ptr<Component>>::iterator ComponentVectorIterator;
        typedef std::unordered_map<std::type_index, ComponentVector> ComponentMap;
        typedef std::unordered_map<std::type_index, ComponentVector>::iterator ComponentMapIterator;

        typedef std::unordered_map<std::type_index, void *> ComponentPoolMap;
        typedef std::unordered_map<std::type_index, size_t> ComponentPoolSize;
        
        SpaceHandle spaceHandle_;
        ComponentMap components_;
        
        template <typename T> void addComponent(std::shared_ptr<T> component) {
            std::type_index index(typeid(T));
            ComponentMapIterator it = components_.find(index);
            
            if (it != components_.end()) {
                // We already have a vector of type T
            } else {
                ComponentVector components = { component };
                components_[index] = components;
            }
        }
    };
}

#endif /* BGEComponentService_h */
