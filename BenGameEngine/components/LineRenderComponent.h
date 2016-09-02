//
//  LineRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGELineRenderComponent_h
#define BGELineRenderComponent_h

#include <stdio.h>
#include "RenderComponent.h"

namespace BGE {
    class LineRenderComponent : public RenderComponent
    {
    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        LineRenderComponent();
        ~LineRenderComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            RenderComponent::initialize(handle, spaceHandle);
            
            thickness_ = 1;
            closedLoop_ = false;
        }
        
        void destroy() final {
            thickness_ = 1;
            points_.clear();
            
            // RenderComponent::destroy last
            RenderComponent::destroy();
        }

        const std::vector<Vector2>& getPoints() const;
        void setPoints(const std::vector<Vector2>& points, bool lineLoop=false);
        bool isLineLoop() const { return closedLoop_; }
        void setLineLoop(bool looped) { closedLoop_ = looped; }
        float getThickness() const { return thickness_; }
        void setThickness(float thickness) { thickness_ = thickness; }
        
    protected:
        void materialsUpdated() {}
        
    private:
        friend ComponentService;

        float thickness_;
        std::vector<Vector2> points_;
        bool closedLoop_;
    };
}

#endif /* BGELineRenderComponent_h */
