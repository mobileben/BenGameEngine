//
//  BGETextureAtlas.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETextureAtlas.h"
#include "BGEGame.h"

BGETextureAtlas::BGETextureAtlas(std::string name) : BGETextureBase(name){
}

std::string BGETextureAtlas::atlasTextureKey() const {
    return "__" + name_ + "_texture";
}

void BGETextureAtlas::releaseCurrentTexture() {
    if (texture_) {
        BGEGame::getInstance()->getTextureService()->removeTexture(textureName_);
        texture_.reset();
    }
    
    textureName_.clear();
    
    BGETextureBase::releaseCurrentTexture();
    
    for (auto& kv : subTextures_) {
        BGEGame::getInstance()->getTextureService()->removeTexture(kv.first);
    }
    
    subTextures_.clear();
}

std::shared_ptr<BGETexture> BGETextureAtlas::getSubTexture(std::string name) {
    return subTextures_[name];
}
