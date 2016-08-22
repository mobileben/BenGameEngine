//
//  Object.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEObject_h
#define BGEObject_h

#include <stdio.h>
#include <string>
#include <memory>

namespace BGE {
    class GameObjectService;
    class NamedObject;
    
    using ObjectId = uint32_t;
    
    class Object
    {
    public:
        Object() : id_(0) {}
        Object(ObjectId objId) : id_(objId) {}
        virtual ~Object() {}
        
        ObjectId getInstanceId() const { return id_; }

    protected:
        void setInstanceId(ObjectId id) {
            id_ = id;
        }
                
    private:
        friend NamedObject;
        
        template <typename DATA, typename HANDLE> friend class HandleService;

        ObjectId    id_;
    };    
}

#endif /* BGEObject_h */
