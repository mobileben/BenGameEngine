//
//  FlatRectRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FlatRectRenderComponent.h"
#include "Game.h"

uint32_t BGE::FlatRectRenderComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::FlatRectRenderComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::FlatRectRenderComponent::type_index_ = typeid(BGE::FlatRectRenderComponent);

BGE::FlatRectRenderComponent::FlatRectRenderComponent() : RenderComponent() {
}

void BGE::FlatRectRenderComponent::setWidth(float width) {
    Vector2 wh;
    
    wh.w = width;
    wh.h = getLocalHeight();
    
    setWidthHeight(wh);
}

void BGE::FlatRectRenderComponent::setHeight(float height) {
    Vector2 wh;

    wh.w = getLocalWidth();
    wh.h = height;

    setWidthHeight(wh);
}

void BGE::FlatRectRenderComponent::setWidthHeight(Vector2 &wh) {
    updateLocalBoundsAndVertices(wh);
}

void BGE::FlatRectRenderComponent::setWidthHeight(Vector2 &&wh) {
    updateLocalBoundsAndVertices(wh);
}

void BGE::FlatRectRenderComponent::materialsUpdated() {
}

void BGE::FlatRectRenderComponent::updateLocalBoundsAndVertices(Vector2& wh) {
    Rect bounds;
    Vertex* const vertices = (Vertex* const) getVertices();
    float x = 0.0F;
    float y = 0.0F;
    
    switch (getAnchor()) {
        case RenderComponentAnchor::Center:
            float w_2 = wh.w / 2.0F;
            float h_2 = wh.h / 2.0F;
            
            bounds.x = x;
            bounds.y = y;
            bounds.w = wh.w;
            bounds.h = wh.h;
            
            setLocalBounds(bounds);
            
            // Setup vertices
            if (Game::getInstance()->getRenderService()->hasInvertedYAxis()) {
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
                 
                 NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
                 
                 */
                
                vertices[0].x = x - w_2;
                vertices[0].y = y + h_2;
                vertices[0].z = 0.0F;
                
                vertices[1].x = x + w_2;
                vertices[1].y = y + h_2;
                vertices[1].z = 0.0F;
                
                vertices[2].x = x + w_2;
                vertices[2].y = y - h_2;
                vertices[2].z = 0.0F;
                
                vertices[3].x = x - w_2;
                vertices[3].y = y - h_2;
                vertices[3].z = 0.0F;
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
                vertices[0].z = 0.0F;
                
                vertices[1].x = x + w_2;
                vertices[1].y = y - h_2;
                vertices[1].z = 0.0F;
                
                vertices[2].x = x + w_2;
                vertices[2].y = y + h_2;
                vertices[2].z = 0.0F;
                
                vertices[3].x = x - w_2;
                vertices[3].y = y + h_2;
                vertices[3].z = 0.0F;
            }
            break;
    }
}

