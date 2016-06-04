//
//  ArrayBuilder.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ArrayBuilder_h
#define ArrayBuilder_h

#include <stdio.h>
#include <cassert>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "FixedArray.h"

namespace BGE {
    template <typename T, typename U>
    class ArrayBuilder;

    template <typename T, typename U>
    class ArrayBuilderIterator {
    public:
        ArrayBuilderIterator(const ArrayBuilder<T, U> *aBuilder, size_t pos) : pos_(pos), aBuilder_(aBuilder) {
        }
        
        bool operator !=(const ArrayBuilderIterator<T, U>& other) const {
            return pos_ != other.pos_;
        }
        
        U& operator* () const {
            return (U&) (*aBuilder_)[pos_];
        }
        
        const ArrayBuilderIterator& operator++() {
            pos_++;
            
            return *this;
        }
        
    private:
        size_t pos_;
        const ArrayBuilder<T, U> *aBuilder_;
    };
    
    template <typename T, typename U>
    class ArrayBuilder {
    public:
        ArrayBuilder() {}
        virtual ~ArrayBuilder() {}
        
        virtual size_t add(const T& item) {
            if (!std::is_same<T, U>::value) {
                assert(false);
            }
            
            auto index = items_.size();
            // Since we know if we are here that they are the same type, do this to allow us to push the item on the stack properly
            U *uitem = (U *)&item;
            
            items_.push_back(*uitem);
            
            return index;
        }
        
        virtual U *createRaw() {
            auto size = items_.size();
            
            if (size == 0) {
                return nullptr;
            }
            
            U *raw = new U[size];
            
            if (raw) {
                memcpy(raw, &items_[0], sizeof(U) * size);
                return raw;
            } else {
                return nullptr;
            }
        }
        
        FixedArray<U> createFixedArray() const {
            return FixedArray<U>((U *)&items_[0], items_.size());
        }
        
        size_t size() const {
            return items_.size();
        }
        
        void clear() {
            items_.clear();
        }
        
        void resize(size_t size) {
            items_.resize(size);
        }
        
        U& operator[](size_t index) {
            return items_[index];
        }
        
        const U& operator[](size_t index) const {
            return items_[index];
        }

        U *addressOf(size_t index) const {
            return (U *) &items_[index];
        }
        
        ArrayBuilderIterator<T, U> begin() const {
            return ArrayBuilderIterator<T, U>(this, 0);
        }
        
        ArrayBuilderIterator<T, U> end() const {
            return ArrayBuilderIterator<T, U>(this, items_.size());
        }
        
    protected:
        std::vector<U> items_;
    };
}


#endif /* ArrayBuilder_h */
