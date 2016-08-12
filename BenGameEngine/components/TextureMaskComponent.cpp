//
//  TextureMaskComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/13/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureMaskComponent.h"
#include "Game.h"

uint32_t BGE::TextureMaskComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::TextureMaskComponent::type_index_ = typeid(BGE::TextureMaskComponent);

std::shared_ptr<BGE::TextureMaskComponent> BGE::TextureMaskComponent::create(ObjectId componentId) {
    return std::make_shared<TextureMaskComponent>(private_key{}, componentId);
}

BGE::TextureMaskComponent::TextureMaskComponent(struct private_key const& key, ObjectId componentId) : RenderComponent(componentId) {
}

void BGE::TextureMaskComponent::setTextureMaskReference(TextureMaskReference *texMaskRef) {
    if (texMaskRef) {
        setTextureMaskReference(*texMaskRef);
    } else {
        NSLog(@"WHAAA");
    }
}

void BGE::TextureMaskComponent::setTextureMaskReference(const TextureMaskReference &texMaskRef) {
    auto material = Game::getInstance()->getMaterialService()->createMaterial(texMaskRef.texture->textureHandle);
    
    this->setMaterials({material});
}

void BGE::TextureMaskComponent::materialsUpdated() {
    
    // Build vertices
    updateLocalBoundsAndVertices();
}

BGE::TextureHandle BGE::TextureMaskComponent::getTextureHandle() {
    auto material = getMaterial();
    
    assert(material);
    
    if (material) {
        return material->getTextureHandle();
    }
    
    return TextureHandle();
}

void BGE::TextureMaskComponent::updateLocalBoundsAndVertices() {
    // Right now we are only supporting one material
    auto material = getMaterial();
    
    assert(material);
    
    if (material) {
        auto textureHandle = material->getTextureHandle();
        auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle);
        
        assert(texture);
        
        if (texture) {
            Rect bounds;
            VertexTex* const vertices = (VertexTex* const) getVertices();
            float x = 0;
            float y = 0;
            float w = texture->getWidth();
            float h = texture->getHeight();
            
            switch (getAnchor()) {
                case RenderComponentAnchor::Center:
                    float w_2 = w / 2.0;
                    float h_2 = h / 2.0;
                    
                    bounds.x = x;
                    bounds.y = y;
                    bounds.w = w;
                    bounds.h = h;
                    
                    setLocalBounds(bounds);
                    
                    const Vector2 *uvs = texture->getUVs();
                    
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