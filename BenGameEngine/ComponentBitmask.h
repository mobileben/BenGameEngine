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
#include <vector>
#include <typeindex>
#include <cassert>

namespace BGE {
    struct ComponentBitmaskElement {
        std::type_index type;
        uint32_t        typeId;
        uint32_t        typeMask;
    };
    
    class ComponentBitmask
    {
    public:
        template <typename T>
        static std::pair<uint32_t, uint32_t> createBitmask() {
            if (index_ < 32) {
                std::type_index index(typeid(T));
                
                auto it = ComponentBitmask::masksOld_.find(index);
                
                if (it != ComponentBitmask::masksOld_.end()) {
                    return it->second;
                } else {
                    uint32_t mask = 1 << index_;
                    ComponentBitmaskElement element{index, index_, mask};
                    
                    assert(masks_.size() == index_);
                    masks_.push_back(element);
                    
                    auto result = std::make_pair(index_, mask);
                    
                    
                    masksOld_[index] = result;
                    
                    index_++;
                    
                    return result;
                }
            } else {
                assert(false);
                return std::make_pair(0, 0);
            }
            
            return std::make_pair(0, 0);
        }

        template <typename T>
        static uint32_t getBitmask() {
            auto const &elem = masks_[T::typeId_];
            
            return elem.typeMask;
        }
        
        static uint32_t bitmaskForTypeIndex(std::type_index index);

    private:
        static uint32_t                                                             index_;
        static std::vector<ComponentBitmaskElement>                                 masks_;
        static std::unordered_map<std::type_index, std::pair<uint32_t, uint32_t>>   masksOld_;
    };
}

#endif /* ComponentBitmask_h */
