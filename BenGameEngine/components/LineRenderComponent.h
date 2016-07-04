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
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;

    public:
        static std::shared_ptr<LineRenderComponent> create(ObjectId componentId);
        
        LineRenderComponent(struct private_key const& key, ObjectId componentId);
        ~LineRenderComponent() {}
        
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
