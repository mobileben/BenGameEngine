//
//  ComponentBitmask.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ComponentBitmask_h
#define ComponentBitmask_h

#include <stdio.h>
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
#include <cassert>

namespace BGE {
    class ComponentBitmask
    {
    public:
        template <typename T>
        static uint32_t createBitmask() {
            if (index_ < 32) {
                std::type_index index(typeid(T));
                
                auto it = ComponentBitmask::masks_.find(index);
                
                if (it != ComponentBitmask::masks_.end()) {
                    return it->second;
                } else {
                    uint32_t mask = 1 << index_;
                    
                    masks_[index] = mask;
                    index_++;
                    
                    printf("XXX Creating bitmask %d\n", mask);
                    return mask;
                }
            } else {
                assert(false);
                return 0;
            }
            
            return 0;
        }
        
    private:
        static uint32_t                                         index_;
        static std::unordered_map<std::type_index, uint32_t>    masks_;
    };
}

#endif /* ComponentBitmask_h */
