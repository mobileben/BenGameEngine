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
    struct SpaceTag {};
    using SpaceHandle = Handle<SpaceTag>;
    
    class SpaceService;
    
    class Space : public NamedObject
    {
    public:
        Space(ObjectId spaceId);
        Space(ObjectId spaceId, std::string name);

        virtual ~Space() {}
        
        void initialize(SpaceHandle handle, ObjectId spaceId, std::string name);

        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            return gameObjectService_->createObject<T>(std::forward<Args>(args)...);
        }
        
        void removeObject(ObjectId objId) { gameObjectService_->removeObject(objId); }
        
        const GameObjectService::GameObjectMap& getGameObjects() const { return gameObjectService_->getGameObjects(); }

        std::shared_ptr<GameObject> find(std::shared_ptr<GameObject> object) { return gameObjectService_->find(object); }
        std::shared_ptr<GameObject> find(ObjectId objId) { return gameObjectService_->find(objId); }
        std::shared_ptr<GameObject> find(std::string name) { return gameObjectService_->find(name); }
        
        template <typename T, typename... Args> std::shared_ptr<T> createComponent(Args&& ...args) {
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            return component;
        }
        
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
        
        std::shared_ptr<GameObject> createAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        std::shared_ptr<GameObject> createAnimChannel(std::string name, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        std::shared_ptr<GameObject> createFrameAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        std::shared_ptr<GameObject> createButton(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        std::shared_ptr<GameObject> createMask(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        std::shared_ptr<GameObject> createSprite(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        std::shared_ptr<GameObject> createText(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        std::shared_ptr<GameObject> createPlacement(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        
        void createAutoDisplayObjects(std::shared_ptr<GameObject> root, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate delegate);

    protected:
        SpaceHandle spaceHandle_;
        
    private:
        friend SpaceService;
        
        std::shared_ptr<GameObjectService> gameObjectService_;
        std::shared_ptr<ComponentService> componentService_;
        std::vector<GameObject> gameObjects_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
