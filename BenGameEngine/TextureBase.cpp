//
//  TextureBase.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextureBase.h"
#include "Game.h"

const std::string BGE::TextureBase::ErrorDomain = "Texture";

BGE::TextureBase::TextureBase(ObjectId texId, std::string name) : NamedObject(texId, name) ,valid_(false), format_(TextureFormat::Undefined), alphaState_(TextureAlphaState::None), width_(0), height_(0) {
    updateUVs();
    updateXYs();
}

void BGE::TextureBase::releaseCurrentTexture() {
    valid_ = false;
    format_ = TextureFormat::Undefined;
    alphaState_ = TextureAlphaState::None;
    x_ = 0;
    y_ = 0;
    width_ = 0;
    height_ = 0;
    
    for (int i=0;i<4;i++) {
        uvs_[i].x = 0;
        uvs_[i].y = 0;
        xys_[i].x = 0;
        xys_[i].y = 0;
    }
}
void BGE::TextureBase::updateUVs(bool rotated) {
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
    
    if (rotated) {
        uvs_[3].x = 0;
        uvs_[3].y = 0;
        uvs_[0].x = 1;
        uvs_[0].y = 0;
        uvs_[1].x = 1;
        uvs_[1].y = 1;
        uvs_[2].x = 0;
        uvs_[2].y = 1;
    } else {
        uvs_[0].x = 0;
        uvs_[0].y = 0;
        uvs_[1].x = 1;
        uvs_[1].y = 0;
        uvs_[2].x = 1;
        uvs_[2].y = 1;
        uvs_[3].x = 0;
        uvs_[3].y = 1;
    }
}

void BGE::TextureBase::updateXYs() {
    if (Game::getInstance()->getRenderService()->hasInvertedYAxis()) {
        /*
         
         +Y down
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 1 - 2
         0 - 2 - 3
         */
        
        xys_[0].x = 0;
        xys_[0].y = getHeight();
        
        xys_[1].x = getWidth();
        xys_[1].y = getHeight();
        
        xys_[2].x = getWidth();
        xys_[2].y = 0;
        
        xys_[3].x = 0;
        xys_[3].y = 0;
    } else {
        /*
         
         +Y up
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 1 - 2
         0 - 2 - 3
         */
        
        xys_[0].x = 0;
        xys_[0].y = 0;
        
        xys_[1].x = getWidth();
        xys_[1].y = 0;
        
        xys_[2].x = getWidth();
        xys_[2].y = getHeight();
        
        xys_[3].x = 0;
        xys_[3].y = getHeight();
    }
}
