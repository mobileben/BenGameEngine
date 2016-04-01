//
//  BGETexture.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETexture_h
#define BGETexture_h

#include <stdio.h>

#include <functional>
#include <string>

#include "BGETextureBase.h"
#include "BGEError.h"

class BGETextureAtlas;

class BGETexture : public BGETextureBase
{
public:
    BGETexture(std::string name);
    
    virtual ~BGETexture() {}
    
    virtual uint32_t getX() const { return x_; }
    virtual uint32_t getY() const { return y_; }
    virtual bool isSubTexture() const { return isSubTexture_; }

    virtual void createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETexture>, std::shared_ptr<BGEError>)> callback) =0;
    virtual std::shared_ptr<BGEError> createSubTexture(std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) =0;
    
    std::weak_ptr<BGETextureAtlas> getTextureAtlas() { return atlas_; }
    
protected:
    uint32_t                        x_;
    uint32_t                        y_;
    bool                            isSubTexture_;
    std::weak_ptr<BGETextureAtlas>  atlas_;

    // Sub-classes should call this
    virtual void releaseCurrentTexture();

    void updateUVs();
};

#endif /* BGETexture_h */
