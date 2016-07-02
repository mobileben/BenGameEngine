//
//  GameObjectService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef GameObjectService_h
#define GameObjectService_h

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include "Service.h"
#include "GameObject.h"
#include <type_traits>

namespace BGE {
    class Space;
    
    class GameObjectService : public BGE::Service {
    public:
        GameObjectService();
        ~GameObjectService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}

        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        Space *getSpace(void) const;
        SpaceHandle getSpaceHandle() const { return spaceHandle_; }

        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            static_assert(std::is_base_of<BGE::GameObject, T>::value, "Not GameObject");
            
            uint64_t objId = getIdAndIncrement();
            std::shared_ptr<T> object = GameObject::create(objId, std::forward<Args>(args)...);
            
            object->setSpaceHandle(spaceHandle_);
            objects_[objId] = object;
            
            return object;
        }
        
        void removeObject(std::shared_ptr<GameObject> object);
        void removeObject(uint64_t objId);
        void removeObject(std::string name);
        
        const std::unordered_map<uint64_t, std::shared_ptr<BGE::GameObject>>& getGameObjects() const { return objects_; }
        
        std::shared_ptr<BGE::GameObject> find(std::shared_ptr<GameObject> object);
        std::shared_ptr<BGE::GameObject> find(uint64_t objId);
        std::shared_ptr<BGE::GameObject> find(std::string name);
        
    private:
        friend Space;
        
        SpaceHandle   spaceHandle_;
        std::unordered_map<uint64_t, std::shared_ptr<BGE::GameObject>> objects_;
    };
}

#endif /* GameObjectService_h */
