//
//  MaskComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MaskComponent.h"
#include "Game.h"
#include "TransformComponent.h"

uint32_t BGE::MaskComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::MaskComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::MaskComponent::type_index_ = typeid(BGE::MaskComponent);

BGE::MaskComponent::MaskComponent() : RenderComponent() {
    Color color = { {1, 0, 1, 1} };
    BGE::MaterialHandle materialHandle;
    
    if (hasMaterials()) {
        auto material = getMaterial();
        if (material) {
            material->setColor(color);
            materialHandle = material->getHandle();
        }
    } else {
        materialHandle = Game::getInstance()->getMaterialService()->createMaterial(color);
    }
    
    this->setMaterial(materialHandle);
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

    // Update the material
    Color color = { {1, 0, 1, 1} };
    BGE::MaterialHandle materialHandle;
    
    if (hasMaterials()) {
        auto material = getMaterial();
        if (material) {
            material->setColor(color);
            materialHandle = material->getHandle();
        }
    } else {
        materialHandle = Game::getInstance()->getMaterialService()->createMaterial(color);
    }
    
    this->setMaterial(materialHandle);
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
    originalSize_ = wh;
    
    updateLocalBoundsAndVertices(wh);
}

void BGE::MaskComponent::reposition(Vector2 position) {
    auto space = getSpace();
    auto gameObj = getGameObject(space);
    auto xform = gameObj->getComponent<TransformComponent>(space);
    
    xform->setPosition(position);
}

void BGE::MaskComponent::resetPosition() {
    reposition(Vector2{{0, 0}});
}

void BGE::MaskComponent::resize(Vector2 size) {
    updateLocalBoundsAndVertices(size);
}

void BGE::MaskComponent::resetSize() {
    updateLocalBoundsAndVertices(originalSize_);
}

void BGE::MaskComponent::materialsUpdated() {
}

void BGE::MaskComponent::updateLocalBoundsAndVertices(Vector2& wh) {
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
