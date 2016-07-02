//
//  TextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureAtlas.h"
#include "Game.h"

BGE::TextureAtlas::TextureAtlas(ObjectId texId, std::string name) : TextureBase(texId, name){
}

std::string BGE::TextureAtlas::atlasTextureKey() const {
    return "__" + getName() + "_texture";
}

void BGE::TextureAtlas::releaseCurrentTexture() {
    if (texture_) {
        Game::getInstance()->getTextureService()->removeTexture(textureName_);
        texture_.reset();
    }
    
    textureName_.clear();
    
    TextureBase::releaseCurrentTexture();
    
    for (auto& kv : subTextures_) {
        Game::getInstance()->getTextureService()->removeTexture(kv.first);
    }
    
    subTextures_.clear();
}

std::shared_ptr<BGE::Texture> BGE::TextureAtlas::getSubTexture(std::string name) {
    return subTextures_[name];
}
