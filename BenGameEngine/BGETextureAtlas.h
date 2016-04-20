//
//  BGETextureAtlas.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureAtlas_h
#define BGETextureAtlas_h

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "BGETextureBase.h"
#include "BGETexture.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;
} BGESubTextureDef;

class BGETextureAtlas : public BGETextureBase
{
public:
    BGETextureAtlas(uint64_t objId, std::string name);
    
    virtual uint32_t getHWTextureId() const =0;
    
    uint32_t getWidth() const { return texture_ ? texture_->getWidth() : 0; }
    uint32_t getHeight() const { return texture_ ? texture_->getHeight() : 0; }

    std::string atlasTextureKey() const;
    std::shared_ptr<BGETexture> getTexture() const { return texture_; };
    std::shared_ptr<BGETexture> getSubTexture(std::string name);
    
    virtual void createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextures, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGE::Error>)> callback) =0;

    void releaseCurrentTexture();

protected:
    std::string textureName_;
    
    // TODO: Make weak?
    std::shared_ptr<BGETexture> texture_;
    // TODO: Make weak?
    std::map<std::string, std::shared_ptr<BGETexture>> subTextures_;
};

#endif /* BGETextureAtlas_h */
