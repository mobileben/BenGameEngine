//
//  Component.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponent_h
#define BGEComponent_h

#include <stdio.h>
#include "Object.h"
#include "Handle.h"
#include "ComponentBitmask.h"
#include <typeindex>

namespace BGE {
    class GameObject;
    class ComponentService;
    class Space;
    
    using ComponentTypeId = uint32_t;
    
    class Component : public Object
    {
    public:
        static uint32_t InvalidBitmask;
        static uint32_t InvalidTypeId;

        /**
         NOTE: When sub-classing, new sub-classes of Component must support bitmask_, type_index_, type_id_. Additionaly register it in
         ComponentService::registerComponents()
         */
        static std::type_index  type_index_;    // Sub-classes must implement
        static ComponentTypeId  typeId_;         // Sub-classes must implement
        static uint32_t         bitmask_;       // Sub-classes must implement
        
        Component() : Object(), handle_(0) {}
        virtual ~Component() {}
        
        virtual void initialize(HandleBackingType handle, SpaceHandle spaceHandle);
        virtual void destroy();

        inline std::type_index getTypeIndex() const { return type_index_; }
        inline ComponentTypeId getTypeId() const { return typeId_; }
        inline uint32_t getBitmask() const { return bitmask_; }
        
        inline bool hasGameObject() const { return !gameObjectHandle_.isNull(); }
        inline GameObjectHandle getGameObjectHandle() const { return gameObjectHandle_; }
        GameObject *getGameObject() const;
        GameObject *getGameObjectLockless() const;
        GameObject *getGameObject(const Space *space) const;
        GameObject *getGameObjectLockless(const Space *space) const;

        Space *getSpace() const;
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }

        template <typename T> inline static uint32_t getBitmask() {
#if DEBUG
            if (!validateComponent<T>()) {
                assert(false);
            }
#endif
            
            return T::bitmask_;
        }
        
        template <typename T> inline static ComponentTypeId getTypeId() {
#if DEBUG
            if (!validateComponent<T>()) {
                assert(false);
            }
#endif
            
            return T::typeId_;
        }

        template <typename T> static inline std::type_index getTypeIndex() { return T::type_index_; }

        template <typename T> inline Handle<T> getHandle() const { return Handle<T>(handle_); }

        inline HandleBackingType getRawHandle() const { return handle_; }
        
    protected:
        template <typename T>
        static void registerBitmask() {
            if (T::bitmask_ == Component::InvalidBitmask) {
                auto results = ComponentBitmask::createBitmask<T>();
                T::bitmask_ = results.second;
                T::typeId_ = results.first;
                T::type_index_ = typeid(T);
            }
        }
        
        template <typename T>
        static bool validateComponent() {
            assert(T::bitmask_ != Component::InvalidBitmask);
            assert(T::typeId_ != Component::InvalidTypeId);
            assert(T::type_index_ != Component::type_index_);
            
            return T::bitmask_ != Component::InvalidBitmask && T::type_index_ != Component::type_index_;
        }
        
        virtual void setGameObjectHandle(GameObjectHandle gameObjectHandle) { gameObjectHandle_ = gameObjectHandle; }
        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }

        virtual void created() {}
        virtual void destroyFast();     // destroyFast is called internally when we are doing removeAllComponents

    private:
        friend ComponentService;
        friend GameObject;
        friend Space;
        
        HandleBackingType   handle_;
        GameObjectHandle    gameObjectHandle_;
        SpaceHandle         spaceHandle_;
    };
}

#endif /* BGEComponent_h */
