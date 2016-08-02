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
#include <cassert>

namespace BGE {
    template <typename TAG>
    class Handle {
    public:
        static const uint32_t NUM_INDEX_BITS = 18;
        static const uint32_t NUM_MAGIC_BITS = 14;
        static const uint32_t MAX_INDEX = (1 << NUM_INDEX_BITS) - 1;
        static const uint32_t MAX_MAGIC = (1 << NUM_MAGIC_BITS) - 1;

        Handle() : handle_(0) {}
        Handle(uint32_t h) : handle_(h) {
            printf("HERE WITH HANDLE CONSTRUCTOR %d\n", h);
        }

        uint32_t getIndex() const { return index_; }
        uint32_t getMagic() const { return magic_; }
        uint32_t getHandle() const { return handle_; }
        
        bool isNull() const { return !handle_; }
        
        inline void init(uint32_t index) {
            assert(isNull());
            assert(index < MAX_INDEX);
            
            static uint32_t autoMagic = 0;
            
            if (++autoMagic > MAX_MAGIC) {
                autoMagic = 1;
            }
            
            index_ = index;
            magic_ = autoMagic;
        }
        
        inline void nullify() {
            handle_ = 0;
        }
        
    private:
        union {
            struct {
                uint32_t index_ : NUM_INDEX_BITS;
                uint32_t magic_: NUM_MAGIC_BITS;
            };
            uint32_t handle_;
        };
    };

    template <typename TAG>
    inline bool operator!= (const Handle<TAG>& lhs, const Handle<TAG>& rhs) { return lhs.getHandle() != rhs.getHandle(); }
    template <typename TAG>
    inline bool operator== (const Handle<TAG>& lhs, const Handle<TAG>& rhs) { return lhs.getHandle() == rhs.getHandle(); }
}

#endif /* Handle_h */
