//
//  TextComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextComponent.h"
#include "Game.h"

uint32_t BGE::TextComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::TextComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::TextComponent::type_index_ = typeid(BGE::TextComponent);

BGE::TextComponent::TextComponent() : RenderComponent() {
}

void BGE::TextComponent::materialsUpdated() {
}

void BGE::TextComponent::setTextReference(const TextReference& textRef) {
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = getSpace()->getGameObject(gameObjHandle);
    auto bbox = gameObj->getComponent<BoundingBoxComponent>();
    text_ = textRef.text;
    color_ = textRef.color;
    horizAlignment_ = textRef.alignment;
    vertAlignment_ = FontVerticalAlignment::Center;
    fontHandle_ = textRef.fontHandle;
    
    auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);
    
    bbox->x = 0;
    bbox->y = 0;
    bbox->width = getWidth();
    bbox->height = font->getHeight();
}

void BGE::TextComponent::setText(std::string const &text) {
    text_ = text;

    updateBoundingBox();
}

void BGE::TextComponent::setFont(FontHandle fontHandle) {
    fontHandle_ = fontHandle;
    
    updateBoundingBox();
}

float BGE::TextComponent::getWidth(bool minimum) {
    auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);
    float width = 0;
    
    if (font) {
        width = font->getStringWidth(text_);
    }
    
    return width;
}

void BGE::TextComponent::getWidthHeight(float &width, float &height) {
    auto font = Game::getInstance()->getFontService()->getFont(fontHandle_);
    
    if (font) {
        width = font->getStringWidth(text_);
        height = font->getHeight();
    } else {
        width = height = 0;
    }
}

void BGE::TextComponent::updateBoundingBox() {
    float width, height;
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = getSpace()->getGameObject(gameObjHandle);
    auto bbox = gameObj->getComponent<BoundingBoxComponent>();
    
    getWidthHeight(width, height);
    
    // TODO: Need to factor in alignment
    bbox->x = 0;
    bbox->y = 0;
    bbox->width = width;
    bbox->height = height;
}


