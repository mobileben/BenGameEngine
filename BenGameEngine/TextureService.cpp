//
//  TextureService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureService.h"

BGE::TextureService::TextureService()
{
    
}

std::shared_ptr<BGE::TextureBase> BGE::TextureService::textureWithName(std::string name) {
    if (this->sTextures_.find(name) != this->sTextures_.end()) {
        return this->sTextures_[name];
    } else {
        return nullptr;
    }
}

std::shared_ptr<BGE::TextureBase> BGE::TextureService::textureWithId(uint64_t texId) {
    if (this->iTextures_.find(texId) != this->iTextures_.end()) {
        return this->iTextures_[texId];
    } else {
        return nullptr;
    }
}

void BGE::TextureService::removeTexture(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<BGE::TextureBase>>::iterator sIt;
    std::unordered_map<uint64_t, std::shared_ptr<BGE::TextureBase>>::iterator iIt;
    
    sIt = sTextures_.find(name);
    
    if (sIt != sTextures_.end()) {
        iIt = iTextures_.find(sIt->second->getInstanceId());
        iTextures_.erase(iIt);
    }
    
    sTextures_.erase(sIt);
}

void BGE::TextureService::removeTexture(uint64_t texId) {
    std::unordered_map<std::string, std::shared_ptr<BGE::TextureBase>>::iterator sIt;
    std::unordered_map<uint64_t, std::shared_ptr<BGE::TextureBase>>::iterator iIt;
    
    iIt = iTextures_.find(texId);
    
    if (iIt != iTextures_.end()) {
        sIt = sTextures_.find(iIt->second->getName());
        sTextures_.erase(sIt);
    }
    
    iTextures_.erase(iIt);
}

void BGE::TextureService::removeTexture(std::shared_ptr<BGE::TextureBase> texture) {
    if (texture) {
        removeTexture(texture->getInstanceId());
    }
}
