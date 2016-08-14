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
    
    class Object : public std::enable_shared_from_this<Object>
    {
    public:
        Object() : id_(0) {}
        Object(ObjectId objId) : id_(objId) {}
        virtual ~Object() {}
        
        ObjectId getInstanceId() const { return id_; }
        
    public:
        template <typename T>
        std::shared_ptr<T> derived_shared_from_this()
        {
            return std::static_pointer_cast<T>(shared_from_this());
        }

    protected:
        void setInstanceId(ObjectId id) {
            id_ = id;
        }
                
    private:
        friend GameObjectService;
        friend NamedObject;
        
        template <typename DATA, typename HANDLE> friend class HandleService;

        ObjectId    id_;
    };    
}

#endif /* BGEObject_h */
