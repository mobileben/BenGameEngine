//
//  PolyLineRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/1/17.
//  Copyright © 2017 2n Productions. All rights reserved.
//

#ifndef PolyLineRenderComponent_h
#define PolyLineRenderComponent_h

#include <stdio.h>
#include "RenderComponent.h"

namespace BGE {
    class PolyLineRenderComponent : public RenderComponent
    {
    public:
        enum class Joint { miter, bevel, round, none };
        enum class Cap { butt, round, square, rect, both, first, last, none };
        
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        PolyLineRenderComponent();
        ~PolyLineRenderComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final {
            RenderComponent::initialize(handle, spaceHandle);
            
            joint_ = Joint::miter;
            cap_ = Cap::both;
            feather_ = false;
            feathering_ = 0;
            noFeatherAtCap_ = false;
            noFeatherAtCore_ = false;
            
            thickness_ = 1;
        }
        
        void destroy() final {
            thickness_ = 1;
            points_.clear();
            colors_.clear();
            
            // RenderComponent::destroy last
            RenderComponent::destroy();
        }
        
        const std::vector<Vector2>& getPoints() const;
        void setPoints(const std::vector<Vector2>& points, bool lineLoop=false);
        const std::vector<Color>& getColors() const;
        void setColors(const std::vector<Color>& colors);
        
        float getThickness() const { return thickness_; }
        void setThickness(float thickness) { thickness_ = thickness; }
        
        Joint getJoint() const { return joint_; }
        void setJoint(Joint joint) { joint_ = joint; }
        
        Cap getCap() const { return cap_; }
        void setCap(Cap cap) { cap_ = cap; }
        
        bool isFeather(void) const { return feather_; }
        void setFeather(bool enable) { feather_ = enable; }
        void setFeather(bool enable, double feathering) { feather_ = enable; feathering_ = feathering; }
        
        double getFeathering() const { return feathering_; }
        void setFeathering(double feathering) { feathering_ = feathering; }
        
        bool getNoFeatherAtCap() const { return noFeatherAtCap_; }
        void setNoFeatherAtCap(bool no) { noFeatherAtCap_ = no; }
        
        bool getNoFeatherAtCore() const { return noFeatherAtCore_; }
        void setNoFeatherAtCore(bool no) { noFeatherAtCore_ = no; }
        
    protected:
        void materialsUpdated() {}
        
    private:
        friend ComponentService;
        
        float                   thickness_;
        std::vector<Vector2>    points_;
        std::vector<Color>      colors_;
        Joint                   joint_;
        Cap                     cap_;
        bool                    feather_;
        double                  feathering_;
        bool                    noFeatherAtCap_;
        bool                    noFeatherAtCore_;
    };
}
#endif /* PolyLineRenderComponent_h */