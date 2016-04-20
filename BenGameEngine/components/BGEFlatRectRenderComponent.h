//
//  BGEFlatRectRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFlatRectRenderComponent_h
#define BGEFlatRectRenderComponent_h

#include <stdio.h>
#include "BGERenderService.h"
#include "BGERenderComponent.h"

class BGERenderService;
class BGERenderServiceOpenGLES2;

class BGEFlatRectRenderComponent : public BGERenderComponent
{
public:
    void setWidth(float width);
    void setHeight(float height);
    void setWidthHeight(BGEVector2 &wh);
    
protected:
    BGEFlatRectRenderComponent(uint32_t componentId);
    BGEFlatRectRenderComponent(uint32_t componentId, std::string name);
    BGEFlatRectRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject);
    
    BGEVertex* const getVertices() { return vertices_; }
    
    void materialsUpdated();

private:
    friend BGEComponentService;
    friend BGERenderService;
    friend BGERenderServiceOpenGLES2;
    
    static const uint32_t NumVertices = 4;
    
    BGEVertex vertices_[NumVertices];
    
    void updateLocalBoundsAndVertices(BGEVector2& wh);
};

#endif /* BGEFlatRectRenderComponent_h */
