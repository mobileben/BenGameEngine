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
    class ComponentService : public BGE::Service
    {
    public:
        ComponentService();
        ~ComponentService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        
        template <typename T> std::shared_ptr<T> getComponent(uint64_t componentId);
        template <typename T> std::shared_ptr<T> getComponent(std::string name);
        
        template <typename T, typename... Args> std::shared_ptr<T> createComponent(Args&& ...args) {
            static_assert(std::is_base_of<Component, T>::value, "Not Component");
            uint64_t objId = getIdAndIncrement();
            std::shared_ptr<T> component = T::create(objId, std::forward<Args>(args)...);
            
            addComponent(component);
            
            return component;
        }
        
        template <typename T> void removeComponent(uint64_t componentId);
        template <typename T> void removeComponent(std::string name);
        template <typename T> void removeAllComponents();
        
    private:
        typedef std::vector<std::shared_ptr<Component>> ComponentVector;
        typedef std::vector<std::shared_ptr<Component>>::iterator ComponentVectorIterator;
        typedef std::unordered_map<std::type_index, ComponentVector> ComponentMap;
        typedef std::unordered_map<std::type_index, ComponentVector>::iterator ComponentMapIterator;

        typedef std::unordered_map<std::type_index, void *> ComponentPoolMap;
        typedef std::unordered_map<std::type_index, size_t> ComponentPoolSize;
        
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
