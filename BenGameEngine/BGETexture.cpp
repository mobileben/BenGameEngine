//
//  BGETexture.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGETexture.h"
#include "BGETextureAtlas.h"
#include "BGEGame.h"

BGETexture::BGETexture(std::string name) : BGETextureBase(name), x_(0), y_(0), isSubTexture_(false) {
}

void BGETexture::releaseCurrentTexture() {
    BGETextureBase::releaseCurrentTexture();
    
    atlas_ .reset();
    isSubTexture_ = false;
}


void BGETexture::updateUVs() {
    if (!isSubTexture_) {
        BGETextureBase::updateUVs();
    } else {
        std::shared_ptr<BGETextureAtlas> atlas = atlas_.lock();
        
        if (atlas) {
            float x = x_;
            float y = y_;
            float width = width_;
            float height = height_;
            float atlasW = atlas->getWidth();
            float atlasH = atlas->getHeight();
            
            /*
             Defaults to OpenGL setup
             
             0 ------- 1
             | \      |
             |  \     |
             |   \    |
             |    \   |
             |     \  |
             |      \ |
             |       \|
             3--------2
             
             */
            uvs_[0].x = x / atlasW;
            uvs_[0].y = y / atlasH;
            uvs_[1].x = (x + width) / atlasW;
            uvs_[1].y = y / atlasH;
            uvs_[2].x = (x + width) / atlasW;
            uvs_[2].y = (y + height) / atlasH;
            uvs_[3].x = x / atlasW;
            uvs_[3].y = (y + height) / atlasH;
        }
    }
}
