//
//  HandleObject.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef HandleObject_h
#define HandleObject_h

#include <stdio.h>
#include "Handle.h"
#include "Object.h"

namespace BGE {

    class HandleObject : public Object
    {
    public:
        HandleObject(uint64_t objId);
        HandleObject(uint64_t objId, std::string name);
        HandleObject(uint64_t objId, std::string name, std::string domain);
        
    private:
        uint32_t    nextFreeHandleIndex_ : Handle::NUM_HANDLE_INDEX_BITS;
        uint32_t    handleCounter_ : Handle::NUM_COUNTER_INDEX_BITS;    //  4K counter, which means 4K-1 stale references to handle can be held
        uint32_t    handleAllocated_ : 1;   //  Handle has been allocated and is deemed a valid handle
        uint32_t    handleInUse_ : 1;       //  Has been initialized and is in use as a valid HandleObject

    };
}

#endif /* HandleObject_h */
