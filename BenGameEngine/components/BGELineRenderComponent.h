//
//  BGELineRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGELineRenderComponent_h
#define BGELineRenderComponent_h

#include <stdio.h>
#include "BGERenderComponent.h"

class BGELineRenderComponent : public BGERenderComponent
{
public:
    const std::vector<BGEVector2>& getPoints() const;
    void setPoints(const std::vector<BGEVector2>& points, bool lineLoop=false);
    bool isLineLoop() const { return closedLoop_; }
    void setLineLoop(bool looped) { closedLoop_ = looped; }
    float getThickness() const { return thickness_; }
    void setThickness(float thickness) { thickness_ = thickness; }
    
protected:
    BGELineRenderComponent(uint32_t componentId);
    BGELineRenderComponent(uint32_t componentId, std::string name);
    BGELineRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject);
    
    void materialsUpdated() {}
    
private:
    friend BGE::ComponentService;
    
    float thickness_;
    std::vector<BGEVector2> points_;
    bool closedLoop_;
};

#endif /* BGELineRenderComponent_h */
