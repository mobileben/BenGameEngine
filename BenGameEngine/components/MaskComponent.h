//
//  MaskComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef MaskComponent_h
#define MaskComponent_h

#include <stdio.h>
#include <memory>
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {
    class MaskComponent : public Component
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;
        
    public:
        static std::shared_ptr<MaskComponent> create(ObjectId componentId);
        
        MaskComponent(struct private_key const& key, ObjectId componentId);
        ~MaskComponent() {}
        
        void setMaskReference(MaskReference *maskRef);
        void setMaskReference(const MaskReference &maskRef);
        
    private:
    };
}

#endif /* MaskComponent_h */
