//
//  SpriteRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpriteRenderComponent.h"
#include "BGEGame.h"

BGE::SpriteRenderComponent::SpriteRenderComponent(uint32_t componentId) : BGE::RenderComponent(componentId) {
}

BGE::SpriteRenderComponent::SpriteRenderComponent(uint32_t componentId, std::string name) : BGE::RenderComponent(componentId, name) {
}

BGE::SpriteRenderComponent::SpriteRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject) : BGE::RenderComponent(componentId, name, gameObject) {
}

void BGE::SpriteRenderComponent::materialsUpdated() {
    
    // Build vertices
    updateLocalBoundsAndVertices();
}


void BGE::SpriteRenderComponent::updateLocalBoundsAndVertices() {
    // Right now we are only supporting one material
    std::shared_ptr<BGE::Material> material = getMaterial().lock();
    
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
                case BGE::RenderComponentAnchor::Center:
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