//
//  InputTouchComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputTouchComponent_h
#define InputTouchComponent_h

#include <stdio.h>
#include "Component.h"

namespace BGE {
    class InputTouchComponent : public Component {
    private:
        struct private_key {};
        friend Component;
        
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        static std::shared_ptr<InputTouchComponent> create(ObjectId componentId);
        
        InputTouchComponent(struct private_key const& key, ObjectId componentId);
        ~InputTouchComponent() {}
    };
}

#endif /* InputTouchComponent_h */
