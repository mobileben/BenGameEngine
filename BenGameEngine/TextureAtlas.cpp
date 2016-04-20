//
//  TextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureAtlas.h"
#include "Game.h"

BGE::TextureAtlas::TextureAtlas(uint64_t texId, std::string name) : BGE::TextureBase(texId, name){
}

std::string BGE::TextureAtlas::atlasTextureKey() const {
    return "__" + getName() + "_texture";
}

void BGE::TextureAtlas::releaseCurrentTexture() {
    if (texture_) {
        BGE::Game::getInstance()->getTextureService()->removeTexture(textureName_);
        texture_.reset();
    }
    
    textureName_.clear();
    
    BGE::TextureBase::releaseCurrentTexture();
    
    for (auto& kv : subTextures_) {
        BGE::Game::getInstance()->getTextureService()->removeTexture(kv.first);
    }
    
    subTextures_.clear();
}

std::shared_ptr<BGE::Texture> BGE::TextureAtlas::getSubTexture(std::string name) {
    return subTextures_[name];
}
