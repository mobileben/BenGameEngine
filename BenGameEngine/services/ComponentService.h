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
        ~ComponentService() {}
        
        static void registerComponents();
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(__attribute__ ((unused)) double deltaTime) final {}
        void garbageCollect() final;

        template <typename T> uint32_t numComponents() const {
            uint32_t num = 0;

            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            num = handleService->numUsedHandles();
            
            return num;
        }

        template <typename T> uint32_t maxComponents() const {
            uint32_t num = 0;
    
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            num = handleService->capacity();
    
            return num;
        }

        template <typename T> uint32_t maxHandlesAllocated() const {
            uint32_t num = 0;
    
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            num = handleService->getMaxAllocated();
    
            return num;
        }

        template <typename T> uint32_t numResizes() const {
            uint32_t num = 0;
    
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            num = handleService->getNumResizes();
    
            return num;
        }

        uint32_t totalNumComponents() const;
        uint32_t totalNumUsedHandles() const;
        uint32_t totalMaxHandlesAllocated() const;
        uint32_t totalMaxHandles() const;

        uint32_t numMaterials() const;
        
        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;

        void outputResourceBreakdown(uint32_t numTabs) const final;
        void outputMemoryBreakdown(uint32_t numTabs) const final;

        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        inline SpaceHandle getSpaceHandle(void) const { return spaceHandle_; }
        
        template <typename T> auto getComponentHandleService() {
            using HANDLE = Handle<T>;
            using HANDLESERVICE = HandleService<T, HANDLE>;
            HANDLESERVICE *handleService = static_cast<HANDLESERVICE *>(componentHandleServices_[T::typeId_]);
            
            return handleService;
        }
        
        template <typename T> auto &getComponentHandles() {
            return componentHandles_[T::typeId_];
        }
        
        template <typename T, typename... Args> T *createComponent() {
            using HANDLE = Handle<T>;
            using HANDLESERVICE = HandleService<T, HANDLE>;
            HANDLESERVICE *handleService = static_cast<HANDLESERVICE *>(componentHandleServices_[T::typeId_]);
            
            if (handleService) {
                lock();
                
                HANDLE componentHandle;
                T *component = handleService->allocate(componentHandle);
                auto &handles = componentHandles_[T::typeId_];
                ComponentHandle handle{T::typeId_, componentHandle.getHandle()};
                
                handles.push_back(handle);
                
                assert(component);
                
                component->initialize(handle.handle, spaceHandle_);
                
                unlock();
                
                return component;
            }
            
            return nullptr;
        }
        
        template <typename T> T *getComponent(HandleBackingType handle) const {
            auto typeId = T::typeId_;
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);
            auto tHandle = Handle<T>(handle);
            return handleService->dereference(tHandle);
        }

        template <typename T> T *getComponent(Handle<T> handle) const {
            auto typeId = T::typeId_;
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);
            return handleService->dereference(handle);
        }

        template <typename T> void getComponents(std::vector<T *> &components) const {
            auto typeId = T::typeId_;
            auto const &handles = componentHandles_[typeId];
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);

            for (auto const &handle : handles) {
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
            
            for (auto const &h : handles) {
                removeComponent(h);
            }
            
            handles.clear();
        }
        
        void removeAllComponents();
        
    private:
        typedef std::unordered_map<std::type_index, void *> ComponentPoolMap;
        typedef std::unordered_map<std::type_index, size_t> ComponentPoolSize;
        
        static bool                 componentsRegistered_;
        static std::vector<void *>  componentHandleServices_;
        static std::vector<std::function<bool(HandleBackingType)>> componentHandleServiceIsHandleBackingNull_;

        SpaceHandle                                 spaceHandle_;
        std::vector<std::vector<ComponentHandle>>   componentHandles_;


        GameObject *getComponentGameObject(Component *, GameObjectHandle gameObjHandle) const;
        
        template <typename T> static void registerComponent(uint32_t reserve, uint32_t maxLimit) {
            using HANDLE = Handle<T>;
            
            Component::registerBitmask<T>();
            auto service = new HandleService<T, HANDLE>(reserve, maxLimit);
            componentHandleServices_.push_back(service);
            componentHandleServiceIsHandleBackingNull_.push_back(std::bind(&HandleService<T, HANDLE>::isHandleBackingNull, service, std::placeholders::_1));
        }

        template <typename T> static void garbageCollectComponent() {
            auto typeId = T::typeId_;
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[typeId]);

            handleService->garbageCollect();
        }
        
        void outputBreakdown(uint32_t numTabs, const char *format, ...) const;

        template <typename T> void outputComponentResourceBreakdown(uint32_t numTabs) const {
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            auto pointers = handleService->activePointers();
            
            assert(pointers.size() == handleService->numUsedHandles());
            
            outputBreakdown(numTabs, "%s: %zd/%d/%d (%d resizes)\n", typeid(T).name(), pointers.size(), handleService->getMaxAllocated(), handleService->capacity(), handleService->getNumResizes());
        }

        template <typename T> void outputComponentMemoryBreakdown(uint32_t numTabs) const {
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            auto pointers = handleService->activePointers();
    
            assert(pointers.size() == handleService->numUsedHandles());
    
            outputBreakdown(numTabs, "%s (sizeof=%zd): %zd/%zd bytes\n", typeid(T).name(), sizeof(T), handleService->usedMemory(), handleService->totalMemory());
        }

        template <typename T> void releaseComponentHandle(ComponentHandle handle) {
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            auto tHandle = Handle<T>(handle.handle);
            auto component = getComponent<T>(handle.handle);

            if (component) {
                component->destroy();
                handleService->release(tHandle);
            }
        }

        // Used with removeAllComponents
        template <typename T> void releaseFastComponentHandle(ComponentHandle handle) {
            auto handleService = static_cast<HandleService<T, Handle<T>> *>(componentHandleServices_[T::typeId_]);
            auto tHandle = Handle<T>(handle.handle);
            auto component = getComponent<T>(handle.handle);

            if (component) {
                component->destroyFast();
                handleService->release(tHandle);
            }
        }
    };
}

#endif /* BGEComponentService_h */
