//
//  UniqueArrayBuilder.hpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef UniqueArrayBuilder_hpp
#define UniqueArrayBuilder_hpp

#include <stdio.h>
#include <cassert>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "FixedArray.h"

namespace BGE {
    template <typename T, typename U>
    class UniqueArrayBuilder {
    public:
        UniqueArrayBuilder() {}
        virtual ~UniqueArrayBuilder() {}
        
        virtual int32_t add(const T& item) {
            if (!std::is_same<T, U>::value) {
                assert(false);
            }
            
            auto index = items_.size();
            auto it = indices_.find(item);
            
            if (it == indices_.end()) {
                U *uitem = (U *)&item;
                items_.push_back(*uitem);
                indices_[item] = index;
            } else {
                index = it->second;
            }
            
            return index;
        }
        
        int32_t indexForItem(const T& item) {
            auto it = indices_[item];
            
            if (it == indices_.end()) {
                return -1;
                items_.push_back(item);
                indices_[item] = index;
            } else {
                return indices_->second;
            }
        }
        
        FixedArray<U> createFixedArray() const {
            return FixedArray<U>((U *)&items_[0], items_.size());
        }
        
        int32_t size() const {
            return (int32_t) items_.size();
        }
        
        void clear() {
            items_.clear();
        }
        
        U& operator[](int32_t index) {
            return items_[index];
        }
        
        const U& operator[](int32_t index) const {
            return items_[index];
        }
        
        U *addressOf(int32_t index) const {
            return (U *) &items_[index];
        }
        
        U *safeAddressOf(int32_t index) const {
            if (index == NullPtrIndex || index >= items_.size()) {
                return nullptr;
            } else {
                return addressOf(index);
            }
        }

    protected:
        std::vector<U> items_;
        std::unordered_map<T, int32_t> indices_;
    };
}

#endif /* UniqueArrayBuilder_hpp */
