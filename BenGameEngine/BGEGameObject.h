//
//  BGEGameObject.hpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEGameObject_hpp
#define BGEGameObject_hpp

#include <stdio.h>
#include "BGEObject.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "BGEComponent.h"

class BGEGameObjectService;

class BGEGameObject : public BGEObject
{
public:
    BGEGameObject(BGEGameObject const&) = delete;
    virtual ~BGEGameObject() {}
    
    template <typename T> std::shared_ptr<T> getComponent() {
        std::type_index index(typeid(T));
        
        if(components_.count(index) != 0) {
            return std::static_pointer_cast<T>(components_[index]);
        } else {
            return nullptr;
        }
    }
    
    template <typename T> void addComponent(std::shared_ptr<T> component) {
        assert(!component->hasGameComponent());
        components_[typeid(T)] = component;
        component->setGameObject(derived_shared_from_this<BGEGameObject>());
    }

    template <typename T> void removeComponent();
    void removeAllComponents();
    
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }
    
private:
    friend BGEGameObjectService;
    
    BGEGameObject(uint64_t objId);
    BGEGameObject(uint64_t objId, std::string name);
    
    bool active_;
    std::unordered_map<std::type_index, std::shared_ptr<BGEComponent>> components_;
};

#endif /* BGEGameObject_hpp */
