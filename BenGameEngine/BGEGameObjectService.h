//
//  BGEGameObjectService.h
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
#include "BGEService.h"
#include "BGEGameObject.h"
#include <type_traits>

class BGEGameObjectService : public BGEService {
public:
    BGEGameObjectService();
    ~BGEGameObjectService();
    
    void initialize() {}
    void reset() {}
    void enteringBackground() {}
    void enteringForeground() {}
    void pause() {}
    void resume() {}
    void destroy() {}

    template < typename T, typename... Args >
    std::shared_ptr< T > createObject(Args&&... args) {
        static_assert(std::is_base_of<BGEGameObject, T>::value, "Not BGEGameObject");
        
        uint64_t objId = getIdAndIncrement();
        std::shared_ptr<T> object(new T(objId, std::forward<Args>(args)...));
        
        objects_[objId] = object;
        
        return object;

    }
    
    void removeObject(uint64_t objId);
    
    std::shared_ptr<BGEGameObject> find(uint64_t objId);
    std::shared_ptr<BGEGameObject> find(std::string name);
    
private:
    std::unordered_map<uint64_t, std::shared_ptr<BGEGameObject>> objects_;
};

#endif /* BGEGameObjectService_h */
