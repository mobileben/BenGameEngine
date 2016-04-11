//
//  BGEComponentService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponentService_h
#define BGEComponentService_h

#include <stdio.h>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "BGEService.h"
#include "BGEComponent.h"

class BGEComponentService : public BGEService
{
public:
    BGEComponentService();
    ~BGEComponentService();
    
    template <typename T> std::shared_ptr<T> getComponent(uint64_t componentId);
    template <typename T> std::shared_ptr<T> getComponent(std::string name);
    template <typename T> void addComponent(std::shared_ptr<T> component);
    template <typename T> void removeComponent(uint64_t componentId);
    template <typename T> void removeComponent(std::string name);
    template <typename T> void removeAllComponents();

private:
    typedef std::vector<std::shared_ptr<BGEComponent>> ComponentVector;
    typedef std::vector<std::shared_ptr<BGEComponent>>::iterator ComponentVectorIterator;
    typedef std::unordered_map<std::type_index, ComponentVector> ComponentMap;
    typedef std::unordered_map<std::type_index, ComponentVector>::iterator ComponentMapIterator;
    
    ComponentMap components_;
};

#endif /* BGEComponentService_h */
