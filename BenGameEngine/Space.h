///Users/ben/Development/BenGameEngine/BenGameEngine/Space.h
//  Space.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpace_h
#define BGESpace_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "NamedObject.h"
#include "GameObjectService.h"
#include "ComponentService.h"
#include "ScenePackage.h"
#include "Handle.h"

namespace BGE {
    class SpaceService;
    
    class Space : public NamedObject
    {
    public:
        Space();
        Space(ObjectId spaceId);
        Space(ObjectId spaceId, std::string name);

        virtual ~Space() {}
        
        void initialize(SpaceHandle handle, ObjectId spaceId, std::string name);

        inline SpaceHandle getHandle() const { return spaceHandle_; }
        
        inline GameObject *createGameObject(std::string name = "") {
            return gameObjectService_->createGameObject(name);
        }
        
        inline GameObjectHandle getGameObjectHandle(ObjectId objId) {
            return gameObjectService_->getGameObjectHandle(objId);
        }
        
        inline GameObjectHandle getGameObjectHandle(std::string name) {
            return gameObjectService_->getGameObjectHandle(name);
        }
        
        inline GameObject *getGameObject(ObjectId objId) {
            return gameObjectService_->getGameObject(objId);
        }
        
        inline GameObject *getGameObject(std::string name) {
            return gameObjectService_->getGameObject(name);
        }
        
        inline GameObject *getGameObject(GameObjectHandle handle) {
            return gameObjectService_->getGameObject(handle);
        }
        
        inline void removeGameObject(GameObjectHandle handle) {
            gameObjectService_->removeGameObject(handle);
        }
        
        const GameObjectService::GameObjectVector& getGameObjects() const { return gameObjectService_->getGameObjects(); }
        
        template <typename COMPONENT, typename HANDLE, typename... Args> COMPONENT *createComponentNew(Args&& ...args) {
            auto component = componentService_->createComponentNew<COMPONENT, HANDLE>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            return component;
        }
        
        TransformComponent * doThis() {
            auto foo = componentService_->tryThis<TransformComponent>();
            
            printf("Foo is %x\n", foo);
            
            return foo;
        }
        template <typename T, typename... Args> std::shared_ptr<T> createComponent(Args&& ...args) {
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            return component;
        }
        
#ifdef NOT_YET
        template <typename T> T *getComponent(ComponentHandle handle) {
            return componentService_->getComponent(ComponentHandle handle);
        }
#endif
        
        template <typename T> std::shared_ptr<T> getComponent(ObjectId componentId) {
            return componentService_->getComponent<T>(componentId);
        }

        template <typename T> void getComponents(std::vector<std::shared_ptr<T>> &components) {
            componentService_->getComponents<T>(components);
        }
        
        template <typename T> void removeComponent(ObjectId componentId) {
            componentService_->removeComponent<T>(componentId);
        }
        
        template <typename T> void removeAllComponents()  {
            componentService_->removeAllComponents<T>();
        }

        void removeComponent(std::type_index typeIndex, ObjectId componentId) {
            componentService_->removeComponent(typeIndex, componentId);
        }
        
        bool isVisible() const { return visible_; }
        void setVisible(bool visible) { visible_ = visible; }
        uint32_t getOrder() const { return order_; }
        void setOrder(uint32_t order) { order_ = order; }
        bool isUpdatable() const { return updatable_; }
        void setUpdatable(bool updatable) { updatable_ = updatable; }
        
        GameObject *createAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createAnimChannel(std::string name, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createFrameAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createButton(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createExternalReference(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createMask(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createSprite(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        GameObject *createText(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        GameObject *createPlacement(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        
        void createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate delegate);

    protected:
        SpaceHandle spaceHandle_;
        
    private:
        friend SpaceService;
        
        std::shared_ptr<GameObjectService> gameObjectService_;
        std::shared_ptr<ComponentService> componentService_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
