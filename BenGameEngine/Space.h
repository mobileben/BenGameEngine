//
//  Space.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpace_h
#define BGESpace_h

#include <stdio.h>
#include "Object.h"
#include "Game.h"
#include "GameObjectService.h"

namespace BGE {
    class SpaceService;
    
    class Space : public Object
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<Space> create(uint64_t spaceId, std::shared_ptr<GameObjectService> gameObjectService);
        static std::shared_ptr<Space> create(uint64_t spaceId, std::string name, std::shared_ptr<GameObjectService> gameObjectService);

        Space(struct private_key const& key, uint64_t spaceId, std::shared_ptr<GameObjectService> gameObjectService);
        Space(struct private_key const& key, uint64_t spaceId, std::string name, std::shared_ptr<GameObjectService> gameObjectService);
        
        virtual ~Space() {}
        
        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            return gameObjectService_->createObject(std::forward<Args>(args)...);
        }
        
        void moveObject(uint64_t objId);
        void removeObject(uint64_t objId) { gameObjectService_->removeObject(objId); }
        
        std::shared_ptr<BGE::GameObject> find(std::shared_ptr<GameObject> object) { return gameObjectService_->find(object); }
        std::shared_ptr<BGE::GameObject> find(uint64_t objId) { return gameObjectService_->find(objId); }
        std::shared_ptr<BGE::GameObject> find(std::string name) { return gameObjectService_->find(name); }

    protected:
        Space() = delete;
        Space(Space const&) = delete;
        
    private:
        friend SpaceService;
        
        std::shared_ptr<GameObjectService> gameObjectService_;
        std::vector<GameObject> gameObjects_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        bool        interactable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
