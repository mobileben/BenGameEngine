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
    struct SpaceTag;
    using SpaceHandle = Handle<SpaceTag>;
    class Space;
    
    class Component : public Object
    {
    private:
        struct private_key {};
        
        /**
         NOTE: When sub-classing, new sub-classes of Component must support bitmask_ and type_index_. Additionaly register it in
         ComponentService::registerComponents()
         */
        static uint32_t         bitmask_;       // Sub-classes must implement
        static std::type_index  type_index_;    // Sub-classes must implement
        
    public:
        static uint32_t InvalidBitmask;
        
        static std::shared_ptr<Component> create(ObjectId componentId);
        
        Component(struct private_key const& key, ObjectId componentId);
        virtual ~Component() {}
        
        bool hasGameObject() const { return !gameObject_.expired(); }
        std::weak_ptr<GameObject> getGameObject() const { return gameObject_; }
        Space *getSpace() const;
        SpaceHandle getSpaceHandle() const { return spaceHandle_; }

        template <typename T>
        static uint32_t getBitmask() {
#if DEBUG
            if (!validateComponent<T>()) {
                T::bitmask_ = ComponentBitmask::createBitmask<T>();
            }
#endif
            
            return T::bitmask_;
        }

        template <typename T>
        static inline std::type_index getTypeIndex() { return T::type_index_; }
        
    protected:
        Component() = delete;
        Component(Component const&) = delete;
        Component(ObjectId componentId);

        template <typename T>
        static void registerBitmask() {
            if (T::bitmask_ == Component::InvalidBitmask) {
                T::bitmask_ = ComponentBitmask::createBitmask<T>();
                T::type_index_ = typeid(T);
            }
        }
        
        template <typename T>
        static bool validateComponent() {
            assert(T::bitmask_ != Component::InvalidBitmask);
            assert(T::type_index_ != Component::type_index_);
            
            return T::bitmask_ != Component::InvalidBitmask && T::type_index_ != Component::type_index_;
        }
        
        void setGameObject(std::shared_ptr<GameObject> gameObject) { gameObject_ = gameObject; }
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }

        virtual void created() {}
        
    private:
        friend ComponentService;
        friend GameObject;
        friend Space;
        
        std::weak_ptr<GameObject>   gameObject_;
        SpaceHandle                 spaceHandle_;
    };
}

#endif /* BGEComponent_h */
