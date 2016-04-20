//
//  GameObjectService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEGameObjectService_h
#define BGEGameObjectService_h

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include "Service.h"
#include "GameObject.h"
#include <type_traits>

namespace BGE {
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
        
        template < typename T, typename... Args >
        std::shared_ptr< T > createObject(Args&&... args) {
            static_assert(std::is_base_of<BGE::GameObject, T>::value, "Not GameObject");
            
            uint64_t objId = getIdAndIncrement();
            std::shared_ptr<T> object(new T(objId, std::forward<Args>(args)...));
            
            objects_[objId] = object;
            
            return object;
        }
        
        void removeObject(uint64_t objId);
        
        const std::unordered_map<uint64_t, std::shared_ptr<BGE::GameObject>>& getGameObjects() const { return objects_; }
        
        std::shared_ptr<BGE::GameObject> find(uint64_t objId);
        std::shared_ptr<BGE::GameObject> find(std::string name);
        
    private:
        std::unordered_map<uint64_t, std::shared_ptr<BGE::GameObject>> objects_;
    };
}

#endif /* BGEGameObjectService_h */
