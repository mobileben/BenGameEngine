//
//  SpriteRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "SpriteRenderComponent.h"
#include "BoundingBoxComponent.h"
#include "Game.h"

uint32_t BGE::SpriteRenderComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::SpriteRenderComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::SpriteRenderComponent::type_index_ = typeid(BGE::SpriteRenderComponent);

BGE::SpriteRenderComponent::SpriteRenderComponent() : RenderComponent() {
}

void BGE::SpriteRenderComponent::setTextureReference(TextureReference *texRef) {
    if (texRef) {
        setTextureReference(*texRef);
    }
}

void BGE::SpriteRenderComponent::setTextureReference(const TextureReference &texRef) {
    BGE::MaterialHandle materialHandle;
    
    if (hasMaterials()) {
        auto material = getMaterial();
        if (material) {
            material->setTextureHandle(texRef.textureHandle);
            materialHandle = material->getHandle();
        }
    } else {
        materialHandle = Game::getInstance()->getMaterialService()->createMaterial(texRef.textureHandle);
    }
    
    if (!materialHandle.isNull()) {
        this->setMaterial(materialHandle);
    }
}

void BGE::SpriteRenderComponent::setTextureHandle(TextureHandle texHandle) {
    BGE::MaterialHandle materialHandle;
    
    if (hasMaterials()) {
        auto material = getMaterial();
        if (material) {
            material->setTextureHandle(texHandle);
            materialHandle = material->getHandle();
        }
    } else {
        materialHandle = Game::getInstance()->getMaterialService()->createMaterial(texHandle);
    }
    
    this->setMaterial(materialHandle);
}

void BGE::SpriteRenderComponent::setTexture(Texture *tex) {
    BGE::MaterialHandle materialHandle;
    
    if (hasMaterials()) {
        auto material = getMaterial();
        if (material) {
            material->setTextureHandle(tex->getHandle());
            materialHandle = material->getHandle();
        }
    } else {
        materialHandle = Game::getInstance()->getMaterialService()->createMaterial(tex->getHandle());
    }
    
    this->setMaterial(materialHandle);
}

BGE::TextureHandle BGE::SpriteRenderComponent::getTextureHandle() {
    auto material = getMaterial();
    
    if (material) {
        return material->getTextureHandle();
    } else {
        return BGE::TextureHandle();
    }
}

void BGE::SpriteRenderComponent::materialsUpdated() {
    // Build vertices
    updateLocalBoundsAndVertices();
}

void BGE::SpriteRenderComponent::updateLocalBoundsAndVertices(bool force) {
    // Right now we are only supporting one material
    auto material = getMaterial();
    
    assert(material);
    
    if (material) {
        auto textureHandle = material->getTextureHandle();
        auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle);
        
        assert(texture);
        
        if (texture) {
            auto gameObjHandle = getGameObjectHandle();
            auto gameObj = getSpace()->getGameObject(gameObjHandle);
            auto bbox = gameObj->getComponent<BoundingBoxComponent>();

            if (BGE::nearlyZero(bbox->width) || BGE::nearlyZero(bbox->height) || force) {
                bbox->x = 0.0F;
                bbox->y = 0.0F;
                bbox->width = texture->getWidth();
                bbox->height = texture->getHeight();
            }

            VertexTex* const vertices = (VertexTex* const) getVertices();
            float x = 0.0F;
            float y = 0.0F;
            float w = bbox->width;
            float h = bbox->height;

            switch (getAnchor()) {
                case RenderComponentAnchor::Center:
                    float w_2 = w / 2.0;
                    float h_2 = h / 2.0;
                    Rect bounds;

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
                        vertices[0].position.z = 0.0F;
                        vertices[0].tex.x = uvs[0].x;
                        vertices[0].tex.y = uvs[0].y;
                        
                        vertices[1].position.x = x + w_2;
                        vertices[1].position.y = y + h_2;
                        vertices[1].position.z = 0.0F;
                        vertices[1].tex.x = uvs[1].x;
                        vertices[1].tex.y = uvs[1].y;
                        
                        vertices[2].position.x = x + w_2;
                        vertices[2].position.y = y - h_2;
                        vertices[2].position.z = 0.0F;
                        vertices[2].tex.x = uvs[2].x;
                        vertices[2].tex.y = uvs[2].y;
                        
                        vertices[3].position.x = x - w_2;
                        vertices[3].position.y = y - h_2;
                        vertices[3].position.z = 0.0F;
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
                        vertices[0].position.z = 0.0F;
                        vertices[0].tex.x = uvs[0].x;
                        vertices[0].tex.y = uvs[0].y;
                        
                        vertices[1].position.x = x + w_2;
                        vertices[1].position.y = y - h_2;
                        vertices[1].position.z = 0.0F;
                        vertices[1].tex.x = uvs[1].x;
                        vertices[1].tex.y = uvs[1].y;
                        
                        vertices[2].position.x = x + w_2;
                        vertices[2].position.y = y + h_2;
                        vertices[2].position.z = 0.0F;
                        vertices[2].tex.x = uvs[2].x;
                        vertices[2].tex.y = uvs[2].y;
                        
                        vertices[3].position.x = x - w_2;
                        vertices[3].position.y = y + h_2;
                        vertices[3].position.z = 0.0F;
                        vertices[3].tex.x = uvs[3].x;
                        vertices[3].tex.y = uvs[3].y;
                    }
                    break;
            }
        }
    }
}
