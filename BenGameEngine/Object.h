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
    
    class Object : public std::enable_shared_from_this<Object>
    {
    public:
        Object(uint64_t objId);
        Object(uint64_t objId, std::string name);
        Object(uint64_t objId, std::string name, std::string domain);
        Object() = delete;
        Object(Object const&) = delete;
        virtual ~Object() {}
        
        uint64_t getInstanceId() const { return id_; }
        std::string getName() const { return name_; }
        void setName(std::string name) { name_ = name; }
        
        bool hasDomain() const { return !domain_.empty(); }
        std::string getDomain() const { return domain_; }
        void setDomain(std::string domain) { domain_ = domain; }
        
        Object& operator=(Object const&) = delete;
        
    public:
        template <typename T>
        std::shared_ptr<T> derived_shared_from_this()
        {
            return std::static_pointer_cast<T>(shared_from_this());
        }
        
    private:
        friend GameObjectService;
        
        uint64_t    id_;
        std::string name_;
        std::string domain_;
    };    
}

#endif /* BGEObject_h */
