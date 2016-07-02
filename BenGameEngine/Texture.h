//
//  Texture.h
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

#include "TextureBase.h"
#include "Error.h"

namespace BGE {
    class TextureAtlas;
    
    class Texture : public TextureBase
    {
    public:
        Texture(ObjectId texId, std::string name);
        
        virtual ~Texture() {}
        
        virtual uint32_t getX() const { return x_; }
        virtual uint32_t getY() const { return y_; }
        virtual bool isSubTexture() const { return isSubTexture_; }
        
        virtual void createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Texture>, std::shared_ptr<Error>)> callback) =0;
        virtual std::shared_ptr<Error> createSubTexture(std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) =0;
        
        std::weak_ptr<TextureAtlas> getTextureAtlas() { return atlas_; }
        
    protected:
        uint32_t                        x_;
        uint32_t                        y_;
        bool                            isSubTexture_;
        std::weak_ptr<TextureAtlas>  atlas_;
        
        // Sub-classes should call this
        virtual void releaseCurrentTexture();
        
        void updateUVs();
    };
}

#endif /* BGETexture_h */
