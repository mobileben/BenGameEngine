//
//  MaskComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MaskComponent.h"
#include "Game.h"

uint32_t BGE::MaskComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::MaskComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::MaskComponent::type_index_ = typeid(BGE::MaskComponent);

BGE::MaskComponent::MaskComponent() : RenderComponent() {
    Color color = { 1, 0, 1, 1 };
    auto handle = Game::getInstance()->getMaterialService()->createMaterial(color);
    setMaterials({handle});
}

void BGE::MaskComponent::setMaskReference(MaskReference *maskRef) {
    if (maskRef) {
        setMaskReference(*maskRef);
    }
}

void BGE::MaskComponent::setMaskReference(const MaskReference &maskRef) {
    Vector2 wh;
    wh.w = maskRef.width;
    wh.h = maskRef.height;
    setWidthHeight(wh);
}

void BGE::MaskComponent::setWidth(float width) {
    Vector2 wh;
    
    wh.w = width;
    wh.h = getLocalHeight();
    
    setWidthHeight(wh);
}

void BGE::MaskComponent::setHeight(float height) {
    Vector2 wh;
    
    wh.w = getLocalWidth();
    wh.h = height;
    
    setWidthHeight(wh);
}

void BGE::MaskComponent::setWidthHeight(Vector2 &wh) {
    wh.w *= 2;
    wh.h *= 2;
    updateLocalBoundsAndVertices(wh);
}

void BGE::MaskComponent::materialsUpdated() {
}

void BGE::MaskComponent::updateLocalBoundsAndVertices(Vector2& wh) {
    Rect bounds;
    Vertex* const vertices = (Vertex* const) getVertices();
    float x = 0;
    float y = 0;
    
    switch (getAnchor()) {
        case RenderComponentAnchor::Center:
            float w_2 = wh.w / 2.0;
            float h_2 = wh.h / 2.0;
            
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
