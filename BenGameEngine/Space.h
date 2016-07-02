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

        std::shared_ptr<GameObjectService> getGameObjectService() const { return gameObjectService_; }
        void setGameObjectService(std::shared_ptr<GameObjectService> gameObjectService) {
            gameObjectService_ = gameObjectService;
        }
        
        std::shared_ptr<ComponentService> getComponentService() const { return componentService_; }
        void setComponentService(std::shared_ptr<ComponentService> componentService) {
            componentService_ = componentService;
        }
        
        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            return gameObjectService_->createObject<T>(std::forward<Args>(args)...);
        }
        
        void moveObject(ObjectId objId);
        void removeObject(ObjectId objId) { gameObjectService_->removeObject(objId); }
        
        const std::unordered_map<ObjectId, std::shared_ptr<GameObject>>& getGameObjects() const { return gameObjectService_->getGameObjects(); }

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

        template <typename T> void removeComponent(ObjectId componentId) {
            componentService_->removeComponent<T>(componentId);
        }
        
        template <typename T> void removeAllComponents()  {
            componentService_->removeAllComponents<T>();
        }

        void removeComponent(std::type_index typeIndex, ObjectId componentId) {
            componentService_->removeComponent(typeIndex, componentId);
        }

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
        bool        interactable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
