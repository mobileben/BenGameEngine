//
//  BGESpriteRenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpriteRenderComponent_h
#define BGESpriteRenderComponent_h

#include <stdio.h>
#include "BGERenderService.h"
#include "BGERenderComponent.h"

class BGERenderService;
class BGERenderServiceOpenGLES2;

class BGESpriteRenderComponent : public BGERenderComponent
{
public:
    std::shared_ptr<BGETextureBase> getTexture();
    
protected:
    BGESpriteRenderComponent(uint32_t componentId);
    BGESpriteRenderComponent(uint32_t componentId, std::string name);
    BGESpriteRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject);

    void materialsUpdated();
    
    BGEVertexTex* const getVertices() { return vertices_; }

private:
    friend BGE::ComponentService;
    friend BGERenderService;
    friend BGERenderServiceOpenGLES2;
    
    static const uint32_t NumVertices = 4;
    
    BGEVertexTex vertices_[NumVertices];
    
    void updateLocalBoundsAndVertices();
};

#endif /* BGESpriteRenderComponent_h */
