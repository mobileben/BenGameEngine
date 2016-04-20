//
//  FlatRectRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FlatRectRenderComponent.h"
#include "BGEGame.h"

BGE::FlatRectRenderComponent::FlatRectRenderComponent(uint32_t componentId) : BGE::RenderComponent(componentId) {
}

BGE::FlatRectRenderComponent::FlatRectRenderComponent(uint32_t componentId, std::string name) : BGE::RenderComponent(componentId, name) {
}

BGE::FlatRectRenderComponent::FlatRectRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject) : BGE::RenderComponent(componentId, name, gameObject) {
}

void BGE::FlatRectRenderComponent::setWidth(float width) {
    BGEVector2 wh;
    
    wh.w = width;
    wh.h = getLocalHeight();
    
    setWidthHeight(wh);
}

void BGE::FlatRectRenderComponent::setHeight(float height) {
    BGEVector2 wh;

    wh.w = getLocalWidth();
    wh.h = height;

    setWidthHeight(wh);
}

void BGE::FlatRectRenderComponent::setWidthHeight(BGEVector2 &wh) {
    updateLocalBoundsAndVertices(wh);
}

void BGE::FlatRectRenderComponent::materialsUpdated() {
}

void BGE::FlatRectRenderComponent::updateLocalBoundsAndVertices(BGEVector2& wh) {
    BGERect bounds;
    BGEVertex* const vertices = (BGEVertex* const) getVertices();
    float x = 0;
    float y = 0;
    
    switch (getAnchor()) {
        case BGE::RenderComponentAnchor::Center:
            float w_2 = wh.w / 2.0;
            float h_2 = wh.h / 2.0;
            
            bounds.x = x;
            bounds.y = y;
            bounds.w = wh.w;
            bounds.h = wh.h;
            
            setLocalBounds(bounds);
            
            // Setup vertices
            if (BGEGame::getInstance()->getRenderService()->hasInvertedYAxis()) {
                /*
                 
                 +Y down
                 
                 0 ------- 1
                 | \      |
                 |  \     |
                 |   \    |
                 |    \   |
                 |     \  |
                 |      \ |
                 |       \|
                 3--------2
                 
                 0 - 1 - 2
                 0 - 2 - 3
                 */
                
                vertices[0].x = x - w_2;
                vertices[0].y = y + h_2;
                vertices[0].z = 0;
                
                vertices[1].x = x + w_2;
                vertices[1].y = y + h_2;
                vertices[1].z = 0;
                
                vertices[2].x = x + w_2;
                vertices[2].y = y - h_2;
                vertices[2].z = 0;
                
                vertices[3].x = x - w_2;
                vertices[3].y = y - h_2;
                vertices[3].z = 0;
            } else {
                /*
                 
                 +Y up
                 
                 0 ------- 1
                 | \      |
                 |  \     |
                 |   \    |
                 |    \   |
                 |     \  |
                 |      \ |
                 |       \|
                 3--------2
                 
                 0 - 2 - 1
                 0 -
                 */
                
                
                vertices[0].x = x - w_2;
                vertices[0].y = y - h_2;
                vertices[0].z = 0;
                
                vertices[1].x = x + w_2;
                vertices[1].y = y - h_2;
                vertices[1].z = 0;
                
                vertices[2].x = x + w_2;
                vertices[2].y = y + h_2;
                vertices[2].z = 0;
                
                vertices[3].x = x - w_2;
                vertices[3].y = y + h_2;
                vertices[3].z = 0;
            }
            break;
    }
}

