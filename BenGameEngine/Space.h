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
#include "Object.h"
#include "GameObjectService.h"
#include "ComponentService.h"

namespace BGE {
    class SpaceService;
    
    class Space : public Object
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<Space> create(uint64_t spaceId, std::shared_ptr<GameObjectService> gameObjectService, std::shared_ptr<ComponentService> componentService);
        static std::shared_ptr<Space> create(uint64_t spaceId, std::string name, std::shared_ptr<GameObjectService> gameObjectService, std::shared_ptr<ComponentService> componentService);
        
        Space(struct private_key const& key, uint64_t spaceId, std::shared_ptr<GameObjectService> gameObjectService, std::shared_ptr<ComponentService> componentService);
        Space(struct private_key const& key, uint64_t spaceId, std::string name, std::shared_ptr<GameObjectService> gameObjectService, std::shared_ptr<ComponentService> componentService);

        virtual ~Space() {}
        
        std::shared_ptr<GameObjectService> getGameObjectService() const { return gameObjectService_; }
        std::shared_ptr<ComponentService> getComponentService() const { return componentService_; }
        
        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            return gameObjectService_->createObject<T>(std::forward<Args>(args)...);
        }
        
        void moveObject(uint64_t objId);
        void removeObject(uint64_t objId) { gameObjectService_->removeObject(objId); }
        
        const std::unordered_map<uint64_t, std::shared_ptr<BGE::GameObject>>& getGameObjects() const { return gameObjectService_->getGameObjects(); }

        std::shared_ptr<BGE::GameObject> find(std::shared_ptr<GameObject> object) { return gameObjectService_->find(object); }
        std::shared_ptr<BGE::GameObject> find(uint64_t objId) { return gameObjectService_->find(objId); }
        std::shared_ptr<BGE::GameObject> find(std::string name) { return gameObjectService_->find(name); }
        
        template <typename T, typename... Args> std::shared_ptr<T> createComponent(Args&& ...args) {
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpace(componentService_->getSpace());
            }
            
            return component;
        }
        
        template <typename T> std::shared_ptr<T> getComponent(uint64_t componentId) {
            return componentService_->getComponent<T>(componentId);
        }
        
        template <typename T> std::shared_ptr<T> getComponent(std::string name) {
            return componentService_->getComponent<T>(name);
        }

        template <typename T> void removeComponent(uint64_t componentId) {
            componentService_->removeComponent<T>(componentId);
        }
        
        template <typename T> void removeComponent(std::string name) {
            componentService_->removeComponent<T>(name);
        }
        
        template <typename T> void removeAllComponents()  {
            componentService_->removeAllComponents<T>();
        }

        void removeComponent(std::type_index typeIndex, uint64_t componentId) {
            componentService_->removeComponent(typeIndex, componentId);
        }
        
        void removeComponent(std::type_index typeIndex, std::string name) {
            componentService_->removeComponent(typeIndex, name);
        }

        bool operator<(const Space& rhs) const { return order_ < rhs.order_; }
        
    protected:
        Space() = delete;
        Space(Space const&) = delete;

    private:
        friend SpaceService;
        
        std::shared_ptr<GameObjectService> gameObjectService_;
        std::shared_ptr<BGE::ComponentService> componentService_;
        std::vector<GameObject> gameObjects_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        bool        interactable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
