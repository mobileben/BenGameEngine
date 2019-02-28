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
    class TransformComponent;
    
    class GameObject : public NamedObject
    {
    public:
        GameObject();
        GameObject(ObjectId objId);
        ~GameObject() {}
        
        void initialize(SpaceHandle spaceHandle, GameObjectHandle gameObjHandle, const std::string& name);
        void markForDestroy();
        void destroy();
        void destroyDontReleaseComponents();
        
        inline GameObjectHandle getHandle() const { return handle_; }
        
        inline bool isActive() const { return active_; }
        void setActive(bool active);

        Space *getSpace() const;
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        
        template <typename T> void addComponent(T *component) {
            assert(!component->hasGameObject());
            auto bitmask = T::bitmask_;
            ComponentHandle handle{T::typeId_, component->getRawHandle()};
            componentBitmask_ |= bitmask;
#if DEBUG
            for (auto &h : components_) {
                assert(h.typeId != T::typeId_);
            }
#endif
            components_.push_back(handle);
            getSpace()->setGameObjectHandle(component, getHandle());
            
            addComponentEpilogue(T::typeId_);
        }
        
        template <typename T> void addComponent(T *component, const Space *space) {
            assert(!component->hasGameObject());
            auto bitmask = T::bitmask_;
            ComponentHandle handle{T::typeId_, component->getRawHandle()};
            componentBitmask_ |= bitmask;
#if DEBUG
            for (auto &h : components_) {
                assert(h.typeId != T::typeId_);
            }
#endif
            components_.push_back(handle);
            space->setGameObjectHandle(component, getHandle());
            
            addComponentEpilogue(T::typeId_);
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
        
        template <typename T> T *getComponent(const Space *space) {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto const &handle : components_) {
                    if (handle.typeId == typeId) {
                        return space->getComponent<T>(handle);
                    }
                }
            }
            
            return nullptr;
        }
        
        template <typename T> T *getComponentLockless(const Space *space) {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto const &handle : components_) {
                    if (handle.typeId == typeId) {
                        return space->getComponentLockless<T>(handle);
                    }
                }
            }
            
            return nullptr;
        }
        
        template <typename T> Handle<T> getComponentHandle() {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto const &handle : components_) {
                    if (handle.typeId == typeId) {
                        return Handle<T>(handle.handle);
                    }
                }
            }
            
            return Handle<T>();
        }
        
        template <typename T> void removeComponent() {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto it = components_.begin();it != components_.end();++it) {
                    if (it->typeId == typeId) {
                        auto space = getSpace();
                        
                        removeComponentPrologue(T::typeId_);
                        
                        space->removeComponent(*it);
                        componentBitmask_ &= ~T::bitmask_;
                        
                        components_.erase(it);
                        return;
                    }
                }
            }
        }
        
        template <typename T> void removeComponent(Space *space) {
            if (hasComponent<T>()) {
                auto typeId = T::typeId_;
                
                for (auto it = components_.begin();it != components_.end();++it) {
                    if (it->typeId == typeId) {
                        removeComponentPrologue(T::typeId_);
                        
                        space->removeComponent(*it);
                        componentBitmask_ &= ~T::bitmask_;
                        
                        components_.erase(it);
                        return;
                    }
                }
            }
        }
        
        void listComponents() const;
        
        void removeAllComponents();
        
        template <typename T>
        GameObject *find(const std::string& name) {
            return find(T::typeId_, name);
        }
        
        template <typename T>
        GameObject *findWithPrefix(const std::string& name) {
            return findWithPrefix(T::typeId_, name);
        }
        
        GameObject *find(ComponentTypeId componentTypeId, const std::string& name);
        GameObject *findWithPrefix(ComponentTypeId componentTypeId, const std::string& name);
        
        GameObject *getParent();
        GameObject *getParent(const Space *space);

        bool hasChildren();
        uint32_t numChildren();

        void addChild(GameObjectHandle handle);
        void addChild(GameObject *child);
        
        void insertChild(GameObjectHandle handle, uint32_t index);
        void insertChild(GameObject *child, uint32_t index);

        void replaceChild(GameObjectHandle handle, uint32_t index);
        void replaceChild(GameObject *child, uint32_t index);

        void moveToParent(GameObjectHandle handle);
        void moveToParent(GameObject *parent);
        
        void removeAllChildren(bool destroy=false);
        void removeFromParent();

        inline uint32_t getComponentBitmask() const { return componentBitmask_; }
        
        template <typename T>
        inline bool hasComponent() const {
            return getComponentBitmask() & Component::getBitmask<T>();
        }
        
        bool isAnimationSubSequence() const;
        void upgradeToAnimationSequence();
        
        // Convenience methods
        inline bool isVisible(void) { return cachedVisibility_; }
        void setVisibility(bool visible);
        
        bool isClipped(void);
        void setClipped(bool clipped);
        
        bool canRender();
        bool canRender(const Space *space);
        bool canInteract();
        bool canInteract(const Space *space);
        bool canInteractLockless(const Space *space);

        inline bool operator==(const GameObject &other) const {
            return getInstanceId() == other.getInstanceId();
        }
        
        inline bool operator!=(const GameObject &other) const {
            return getInstanceId() != other.getInstanceId();
        }
        
    private:
        friend GameObjectService;
        friend TransformComponent;
        
        bool                            active_;
        bool                            cachedVisibility_;
        bool                            destroy_;
        uint32_t                        componentBitmask_;
        GameObjectHandle                handle_;
        SpaceHandle                     spaceHandle_;
        std::vector<ComponentHandle>    components_;
        
        void setCachedVisibility(bool visible) {
            cachedVisibility_ = visible;
        }
        
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        
        void addComponentEpilogue(ComponentTypeId componentTypeId);
        void removeComponentPrologue(ComponentTypeId componentTypeId);
    };
}

#endif /* GameObject_hpp */
