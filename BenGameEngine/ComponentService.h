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

namespace BGE {
    class Space;
    class GameObject;
    
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

        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        inline SpaceHandle getSpaceHandle(void) const { return spaceHandle_; }
        
        template <typename T, typename... Args> T *createComponent(Args&& ...args) {
            using HANDLE = Handle<T>;
            using HANDLESERVICE = HandleService<T, HANDLE>;
            HANDLESERVICE *handleService = static_cast<HANDLESERVICE *>(componentHandleServices_[T::typeId_]);
            
            if (handleService) {
                HANDLE componentHandle;
                T *component = handleService->allocate(componentHandle);
                auto &handles = componentHandles_[T::typeId_];
                ComponentHandle handle{T::typeId_, componentHandle.getHandle()};
                
                handles.push_back(handle);
                
                assert(component);
                
                component->initialize(handle.handle, spaceHandle_);
                
                return component;
            }
            
            return nullptr;
        }
        
        template <typename T> T *getComponent(HandleBackingType handle) {
            auto typeId = T::typeId_;
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);
            auto tHandle = Handle<T>(handle);
            return handleService->dereference(tHandle);
        }
        
        template <typename T> void getComponents(std::vector<T *> &components) {
            auto typeId = T::typeId_;
            auto &handles = componentHandles_[typeId];
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);
            
            components.clear();

            for (auto handle : handles) {
                auto component = handleService->dereference(Handle<T>(handle.handle));
                
                if (component) {
                    components.push_back(component);
                }
            }
        }
        
        void removeComponent(ComponentHandle handle);
        
        template <typename T> void removeAllComponents() {
            auto typeId = T::typeId_;
            auto &handles = componentHandles_[typeId];
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);
            
            for (auto h : handles) {
                handleService->release(Handle<T>(h.handle));
            }
        }
        
    private:
        typedef std::unordered_map<std::type_index, void *> ComponentPoolMap;
        typedef std::unordered_map<std::type_index, size_t> ComponentPoolSize;
        
        static bool                 componentsRegistered_;
        static std::vector<void *>  componentHandleServices_;
        
        SpaceHandle                                 spaceHandle_;
        std::vector<std::vector<ComponentHandle>>   componentHandles_;
        
        GameObject *getComponentGameObject(Component *, GameObjectHandle gameObjHandle);
        
        template <typename T> static void registerComponent(uint32_t reserve, uint32_t maxLimit) {
            using HANDLE = Handle<T>;
            
            Component::registerBitmask<T>();
            componentHandleServices_.push_back(new HandleService<T, HANDLE>(reserve, maxLimit));
        }
        
        template <typename T> void releaseComponentHandle(ComponentHandle handle) {
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            auto tHandle = Handle<T>(handle.handle);
            handleService->release(tHandle);
        }
    };
}

#endif /* BGEComponentService_h */
