//
//  BGETextureService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGETextureService.h"

BGETextureService::BGETextureService()
{
    
}

std::shared_ptr<BGETextureBase> BGETextureService::textureWithName(std::string name) {
    return this->textures_[name];
}

void BGETextureService::removeTexture(std::string name) {
    std::map<std::string, std::shared_ptr<BGETextureBase>>::iterator it;
    
    it = textures_.find(name);
    textures_.erase(it);
}

void BGETextureService::removeTexture(std::shared_ptr<BGETextureBase> texture) {
    if (texture) {
        removeTexture(texture->getName());
    }
}
