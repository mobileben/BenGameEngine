//
//  Handle.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Handle_h
#define Handle_h

#include <stdio.h>
#include <cstdint>

namespace BGE {
    struct Handle {
        static const uint32_t NUM_HANDLE_INDEX_BITS = 18;
        static const uint32_t NUM_COUNTER_INDEX_BITS = 12;
        
        Handle() : index_(0), counter_(0) {
        }
        Handle(uint32_t index, uint32_t counter) : index_(index), counter_(counter) {
        }
        
        inline operator uint32_t() const;
        uint32_t index_ : NUM_HANDLE_INDEX_BITS;
        uint32_t counter_: NUM_COUNTER_INDEX_BITS;
    };
    
    Handle::operator uint32_t() const {
        return counter_ << NUM_HANDLE_INDEX_BITS | index_;
    }

    inline bool operator!=(const Handle& lhs, const Handle& rhs) { return lhs != rhs; }
}

#endif /* Handle_h */
