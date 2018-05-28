//
//  LogicComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/24/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef LogicComponent_h
#define LogicComponent_h

#include <stdio.h>
#include <functional>
#include "Component.h"
#include "Handle.h"

namespace BGE {
    class GameObject;
    class Space;
    
#if TARGET_OS_IPHONE
    
    typedef void (*ObjCLogicComponentFunction)(__weak id, SEL, GameObject *, double);
    
#endif /* TARGET_OS_IPHONE */

    using LogicComponentFunction = std::function<void(GameObject *gameObject, double deltaTime)>;
    
    enum class LogicComponentMode {
        Always,
        Unapaused,
        Paused
    };
    
    class LogicComponent : public Component {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;

        // TODO: support mode and priority
        LogicComponentMode  mode;
        uint32_t            priority;   // Higher number is higher priority
        
        LogicComponentFunction update;
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        void destroy() final;

        LogicComponent();
        ~LogicComponent() {}

    protected:
        void destroyFast() final;

    private:
        friend ComponentService;
    };
}

#endif /* LogicComponent_h */
