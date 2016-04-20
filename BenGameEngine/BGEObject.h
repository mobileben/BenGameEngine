//
//  BGEObject.h
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

class BGEGameObjectService;

class BGEObject : public std::enable_shared_from_this<BGEObject>
{
public:
    BGEObject(uint64_t objId);
    BGEObject(uint64_t objId, std::string name);
    BGEObject(uint64_t objId, std::string name, std::string domain);
    BGEObject() = delete;
    BGEObject(BGEObject const&) = delete;
    virtual ~BGEObject() {}

    uint64_t getInstanceId() const { return id_; }
    std::string getName() const { return name_; }
    void setName(std::string name) { name_ = name; }
    
    bool hasDomain() const { return !domain_.empty(); }
    std::string getDomain() const { return domain_; }
    void setDomain(std::string domain) { domain_ = domain; }
    
    BGEObject& operator=(BGEObject const&) = delete;
    
public:
    template <typename T>
    std::shared_ptr<T> derived_shared_from_this()
    {
        return std::static_pointer_cast<T>(shared_from_this());
    }

private:
    friend BGEGameObjectService;
    
    uint64_t    id_;
    std::string name_;
    std::string domain_;
};

#endif /* BGEObject_h */
