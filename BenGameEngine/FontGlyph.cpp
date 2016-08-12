//
//  FontGlyph.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FontGlyph.h"
#include "Font.h"
#include "Game.h"
#include "Texture.h"

BGE::FontGlyph::FontGlyph() : offsetX_(0), offsetY_(0), offsetYInverted_(0), advance_(0) {
}

BGE::FontGlyph::FontGlyph(Font *font, TextureHandle textureHandle, int32_t offsetX, int32_t offsetY, int32_t advance) : textureHandle_(textureHandle), offsetX_(offsetX), offsetY_(0), offsetYInverted_(offsetY), advance_(advance) {
    auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle);
    
    if (texture) {
        offsetY_ = font->getGlyphH() - (texture->getHeight() + offsetY);
    }
}

int32_t BGE::FontGlyph::getOffsetY() const {
    if (Game::getInstance()->getRenderService()->hasInvertedYAxis()) {
        return offsetY_;
    } else {
        return offsetYInverted_;
    }
}
