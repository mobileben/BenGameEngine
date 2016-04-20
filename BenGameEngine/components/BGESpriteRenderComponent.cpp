//
//  BGESpriteRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGESpriteRenderComponent.h"
#include "BGEGame.h"

BGESpriteRenderComponent::BGESpriteRenderComponent(uint32_t componentId) : BGERenderComponent(componentId) {
}

BGESpriteRenderComponent::BGESpriteRenderComponent(uint32_t componentId, std::string name) : BGERenderComponent(componentId, name) {
}

BGESpriteRenderComponent::BGESpriteRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject) : BGERenderComponent(componentId, name, gameObject) {
}

void BGESpriteRenderComponent::materialsUpdated() {
    
    // Build vertices
    updateLocalBoundsAndVertices();
}


void BGESpriteRenderComponent::updateLocalBoundsAndVertices() {
    // Right now we are only supporting one material
    std::shared_ptr<BGEMaterial> material = getMaterial().lock();
    
    assert(material);
    
    if (material) {
        std::shared_ptr<BGETextureBase> texture = material->getTexture().lock();
        
        assert(texture);
        
        if (texture) {
            BGERect bounds;
            BGEVertexTex* const vertices = (BGEVertexTex* const) getVertices();
            float x = 0;
            float y = 0;
            float w = texture->getWidth();
            float h = texture->getHeight();
            
            switch (getAnchor()) {
                case BGERenderComponentAnchor::Center:
                    float w_2 = w / 2.0;
                    float h_2 = h / 2.0;
                    
                    bounds.x = x;
                    bounds.y = y;
                    bounds.w = w;
                    bounds.h = h;
                    
                    setLocalBounds(bounds);
                    
                    const BGEVector2 *xys = texture->getXYs();
                    const BGEVector2 *uvs = texture->getUVs();
                    
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
                        
                        vertices[0].position.x = x - w_2;
                        vertices[0].position.y = y + h_2;
                        vertices[0].position.z = 0;
                        vertices[0].tex.x = uvs[0].x;
                        vertices[0].tex.y = uvs[0].y;
                        
                        vertices[1].position.x = x + w_2;
                        vertices[1].position.y = y + h_2;
                        vertices[1].position.z = 0;
                        vertices[1].tex.x = uvs[1].x;
                        vertices[1].tex.y = uvs[1].y;
                        
                        vertices[2].position.x = x + w_2;
                        vertices[2].position.y = y - h_2;
                        vertices[2].position.z = 0;
                        vertices[2].tex.x = uvs[2].x;
                        vertices[2].tex.y = uvs[2].y;
                        
                        vertices[3].position.x = x - w_2;
                        vertices[3].position.y = y - h_2;
                        vertices[3].position.z = 0;
                        vertices[3].tex.x = uvs[3].x;
                        vertices[3].tex.y = uvs[3].y;
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
                         
                         0 - 1 - 2
                         0 - 2 - 3
                         */
                        
                        vertices[0].position.x = x - w_2;
                        vertices[0].position.y = y - h_2;
                        vertices[0].position.z = 0;
                        vertices[0].tex.x = uvs[0].x;
                        vertices[0].tex.y = uvs[0].y;
                        
                        vertices[1].position.x = x + w_2;
                        vertices[1].position.y = y - h_2;
                        vertices[1].position.z = 0;
                        vertices[1].tex.x = uvs[1].x;
                        vertices[1].tex.y = uvs[1].y;
                        
                        vertices[2].position.x = x + w_2;
                        vertices[2].position.y = y + h_2;
                        vertices[2].position.z = 0;
                        vertices[2].tex.x = uvs[2].x;
                        vertices[2].tex.y = uvs[2].y;
                        
                        vertices[3].position.x = x - w_2;
                        vertices[3].position.y = y + h_2;
                        vertices[3].position.z = 0;
                        vertices[3].tex.x = uvs[3].x;
                        vertices[3].tex.y = uvs[3].y;
                    }
                    break;
            }
        }
    }
}