//
//  BGETextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETextureAtlas.h"
#include "Game.h"

BGETextureAtlas::BGETextureAtlas(uint64_t texId, std::string name) : BGETextureBase(texId, name){
}

std::string BGETextureAtlas::atlasTextureKey() const {
    return "__" + getName() + "_texture";
}

void BGETextureAtlas::releaseCurrentTexture() {
    if (texture_) {
        BGE::Game::getInstance()->getTextureService()->removeTexture(textureName_);
        texture_.reset();
    }
    
    textureName_.clear();
    
    BGETextureBase::releaseCurrentTexture();
    
    for (auto& kv : subTextures_) {
        BGE::Game::getInstance()->getTextureService()->removeTexture(kv.first);
    }
    
    subTextures_.clear();
}

std::shared_ptr<BGETexture> BGETextureAtlas::getSubTexture(std::string name) {
    return subTextures_[name];
}
