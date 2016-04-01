//
//  BGEFontGlyph.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEFontGlyph.h"
#include "BGEFont.h"
#include "BGEGame.h"
#include "BGETexture.h"

BGEFontGlyph::BGEFontGlyph(BGEFont *font, std::shared_ptr<BGETexture> texture, int32_t offsetX, int32_t offsetY, int32_t advance) : texture_(texture), offsetX_(offsetX), offsetY_(0), offsetYInverted_(offsetY), advance_(advance) {
    if (texture) {
        offsetY_ = font->getGlyphH() - (texture->getHeight() + offsetY);
    }
}

int32_t BGEFontGlyph::getOffsetY() const {
    if (BGEGame::getInstance()->getRenderService()->hasInvertedYAxis()) {
        return offsetY_;
    } else {
        return offsetYInverted_;
    }
}
