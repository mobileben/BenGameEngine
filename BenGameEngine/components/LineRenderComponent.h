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
    class LineRenderComponent : public BGE::RenderComponent
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<LineRenderComponent> create(uint64_t componentId);
        static std::shared_ptr<LineRenderComponent> create(uint64_t componentId, std::string name);
        
        LineRenderComponent(struct private_key const& key, uint64_t componentId);
        LineRenderComponent(struct private_key const& key, uint64_t componentId, std::string name);
        virtual ~LineRenderComponent() {}
        
        const std::vector<BGEVector2>& getPoints() const;
        void setPoints(const std::vector<BGEVector2>& points, bool lineLoop=false);
        bool isLineLoop() const { return closedLoop_; }
        void setLineLoop(bool looped) { closedLoop_ = looped; }
        float getThickness() const { return thickness_; }
        void setThickness(float thickness) { thickness_ = thickness; }
        
    protected:
        LineRenderComponent(uint32_t componentId);
        LineRenderComponent(uint32_t componentId, std::string name);
        
        void materialsUpdated() {}
        
    private:
        friend BGE::ComponentService;
        
        float thickness_;
        std::vector<BGEVector2> points_;
        bool closedLoop_;
    };
}

#endif /* BGELineRenderComponent_h */
