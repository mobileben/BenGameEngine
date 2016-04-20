//
//  BGETextureService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETextureService.h"

BGETextureService::BGETextureService()
{
    
}

std::shared_ptr<BGETextureBase> BGETextureService::textureWithName(std::string name) {
    return this->sTextures_[name];
}

std::shared_ptr<BGETextureBase> BGETextureService::textureWithId(uint64_t texId) {
    return this->iTextures_[texId];
}

void BGETextureService::removeTexture(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator sIt;
    std::unordered_map<uint64_t, std::shared_ptr<BGETextureBase>>::iterator iIt;
    
    sIt = sTextures_.find(name);
    
    if (sIt != sTextures_.end()) {
        iIt = iTextures_.find(sIt->second->getInstanceId());
        iTextures_.erase(iIt);
    }
    
    sTextures_.erase(sIt);
}

void BGETextureService::removeTexture(uint64_t texId) {
    std::unordered_map<std::string, std::shared_ptr<BGETextureBase>>::iterator sIt;
    std::unordered_map<uint64_t, std::shared_ptr<BGETextureBase>>::iterator iIt;
    
    iIt = iTextures_.find(texId);
    
    if (iIt != iTextures_.end()) {
        sIt = sTextures_.find(iIt->second->getName());
        sTextures_.erase(sIt);
    }
    
    iTextures_.erase(iIt);
}

void BGETextureService::removeTexture(std::shared_ptr<BGETextureBase> texture) {
    if (texture) {
        removeTexture(texture->getInstanceId());
    }
}
