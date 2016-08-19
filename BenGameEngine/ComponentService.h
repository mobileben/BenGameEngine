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
#include "HandleService.h"
#include "GameObject.h"

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
#if DEBUG
            static_assert(std::is_base_of<Component, T>::value, "Not Component");
#endif
            ObjectId objId = getIdAndIncrement();
            std::shared_ptr<T> component = T::create(objId, std::forward<Args>(args)...);
            
            component->setSpaceHandle(spaceHandle_);
            addComponent(component);
            
            component->created();
            
            return component;
        }
        
        template <typename T> T *tryThis() {
            Handle<T> handle;
            T *foo = foo_.allocate(handle);
            
            return foo;
        }
        
        template <typename COMPONENT, typename HANDLE, typename... Args> COMPONENT *createComponentNew(Args&& ...args) {
            HandleService<COMPONENT, HANDLE> *handleService = static_cast<HandleService<COMPONENT, HANDLE> *>(componentHandleServices_[COMPONENT::typeId_]);
            
            if (handleService) {
                HANDLE componentHandle;
                COMPONENT *component = handleService->allocate(componentHandle);
                auto &handles = componentHandles_[COMPONENT::typeId_];
                ComponentHandle handle{COMPONENT::bitmask_, componentHandle.getHandle()};
                component->initialize(handle.handle);
                
                handles.push_back(handle);
                return component;
            }
            
            return nullptr;
        }
        
        template <typename T> T *getComponent(ComponentHandle handle) {
            
        }
        
        template <typename T> std::shared_ptr<T> getComponent(ObjectId componentId);
        template <typename T> void getComponents(std::vector<std::shared_ptr<T>> &components) {
            components.clear();
            
            ComponentMapIterator it = components_.find(Component::getTypeIndex<T>());
            
            if (it != components_.end()) {
                for (auto component : it->second) {
                    auto gameObjHandle = component->getGameObjectHandle();
                    auto gameObj = getComponentGameObject(component.get(), gameObjHandle);
                    if (gameObj->isActive()) {
                        auto z = std::static_pointer_cast<T>(component);
                        components.push_back(z);
                    }
                }
            }
        }
        
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
        
        static bool componentsRegistered_;
        static std::vector<std::function<void *(uint32_t, uint32_t)>> handleServiceCreators_;
        
        SpaceHandle spaceHandle_;
        ComponentMap components_;
        std::vector<std::vector<ComponentHandle>>   componentHandles_;
        std::vector<void *>                         componentHandleServices_;
        
        HandleService<TransformComponent, TransformComponent2Handle> foo_;
        
        GameObject *getComponentGameObject(Component *, GameObjectHandle gameObjHandle);
        
        template <typename T> void addComponent(std::shared_ptr<T> component) {
            std::type_index index = Component::getTypeIndex<T>();
            ComponentMapIterator it = components_.find(index);
            
            if (it != components_.end()) {
                // We already have a vector of type T
                it->second.push_back(component);
            } else {
                ComponentVector components = { component };
                components_[index] = components;
            }
        }
        
        template <typename COMPONENT, typename HANDLE> static void registerComponent() {
            Component::registerBitmask<COMPONENT>();
            handleServiceCreators_.push_back(std::bind(&HandleService<COMPONENT, HANDLE>::createService, std::placeholders::_1, std::placeholders::_2));
        }
    };
}

#endif /* BGEComponentService_h */
