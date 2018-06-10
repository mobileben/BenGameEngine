//
//  TextureMaskComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/13/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureMaskComponent.h"
#include "Game.h"
#include "TransformComponent.h"

uint32_t BGE::TextureMaskComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::TextureMaskComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::TextureMaskComponent::type_index_ = typeid(BGE::TextureMaskComponent);

BGE::TextureMaskComponent::TextureMaskComponent() : RenderComponent() {
}

void BGE::TextureMaskComponent::setTextureMaskReference(TextureMaskReference *texMaskRef) {
    if (texMaskRef) {
        setTextureMaskReference(*texMaskRef);
    } else {
        printf("WHAAA\n");
    }
}

void BGE::TextureMaskComponent::setTextureMaskReference(const TextureMaskReference &texMaskRef) {
    auto material = Game::getInstance()->getMaterialService()->createMaterial(texMaskRef.texture->textureHandle);
    this->setMaterials({material});

    auto texture = Game::getInstance()->getTextureService()->getTexture(texMaskRef.texture->textureHandle);
    Vector2 wh;
    if (texture) {
        wh.w = texture->getWidth();
        wh.h = texture->getHeight();
    } else {
        wh.w = wh.h = 0;
    }

    setWidthHeight(wh);
}

void BGE::TextureMaskComponent::setWidth(float width) {
    Vector2 wh;

    wh.w = width;
    wh.h = getLocalHeight();

    setWidthHeight(wh);
}

void BGE::TextureMaskComponent::setHeight(float height) {
    Vector2 wh;

    wh.w = getLocalWidth();
    wh.h = height;

    setWidthHeight(wh);
}

void BGE::TextureMaskComponent::setWidthHeight(Vector2 &wh) {
    originalSize_ = wh;

    updateLocalBoundsAndVertices(wh);
}

void BGE::TextureMaskComponent::reposition(Vector2 position) {
    auto gameObj = getGameObject();
    auto xform = gameObj->getComponent<TransformComponent>();

    xform->setPosition(position);
}

void BGE::TextureMaskComponent::resetPosition() {
    auto gameObj = getGameObject();
    auto xform = gameObj->getComponent<TransformComponent>();

    xform->setPosition(Vector2{0, 0});
}

void BGE::TextureMaskComponent::resize(Vector2 size) {
    updateLocalBoundsAndVertices(size);
}

void BGE::TextureMaskComponent::resetSize() {
    updateLocalBoundsAndVertices(originalSize_);
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

void BGE::TextureMaskComponent::updateLocalBoundsAndVertices(Vector2& wh) {
    Rect bounds;
    VertexTex* const vertices = (VertexTex* const) getVertices();
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

                vertices[0].position.x = x - w_2;
                vertices[0].position.y = y + h_2;
                vertices[0].position.z = 0;

                vertices[1].position.x = x + w_2;
                vertices[1].position.y = y + h_2;
                vertices[1].position.z = 0;

                vertices[2].position.x = x + w_2;
                vertices[2].position.y = y - h_2;
                vertices[2].position.z = 0;

                vertices[3].position.x = x - w_2;
                vertices[3].position.y = y - h_2;
                vertices[3].position.z = 0;
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


                vertices[0].position.x = x - w_2;
                vertices[0].position.y = y - h_2;
                vertices[0].position.z = 0;

                vertices[1].position.x = x + w_2;
                vertices[1].position.y = y - h_2;
                vertices[1].position.z = 0;

                vertices[2].position.x = x + w_2;
                vertices[2].position.y = y + h_2;
                vertices[2].position.z = 0;

                vertices[3].position.x = x - w_2;
                vertices[3].position.y = y + h_2;
                vertices[3].position.z = 0;
            }
            break;
    }
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
