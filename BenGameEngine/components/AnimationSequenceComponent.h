//
//  AnimationSequenceComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AnimationSequenceComponent_h
#define AnimationSequenceComponent_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Component.h"
#include "GraphicFormats.h"
#include "Space.h"

namespace BGE {

    class AnimationChannelComponent;
    
    class AnimationSequenceComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        AnimationSequenceComponent();
        ~AnimationSequenceComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        void destroy() final;

        // TODO: Handle to AnimationSequenceReference
        uint32_t    frameRate;
        uint32_t    totalFrames;
        uint32_t    numChannels;
        uint32_t    numBounds;
        
        // AnimationChannel
        
        //  Pointer to meta
        std::vector<GameObjectHandle> channels;
        BoundsReference *bounds;
        double duration() const { return frameRate ? (1.0 / static_cast<double>(frameRate)) * static_cast<double>(totalFrames) : 0; }
        
    protected:
        friend GameObject;
        friend ComponentService;
        friend Space; // We allow Space to access us because we need a 3rd Party to set AnimationSequenceReference since this can cause the pointer to change
        
        void initialize(HandleBackingType handle);
    };
}

#endif /* AnimationComponent_h */
