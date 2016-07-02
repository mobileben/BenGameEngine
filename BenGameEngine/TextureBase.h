//
//  TextureBase.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureBase_h
#define BGETextureBase_h

#include <stdio.h>
#include <string>
#include "NamedObject.h"
#include "MathTypes.h"

typedef enum {
    TextureErrorNone = 0,
    TextureErrorOS,
    TextureErrorNoBuffer,
    TextureErrorZeroWidthOrHeight,
    TextureErrorClassAllocation,
    TextureErrorAllocation,
    TextureErrorInvalidSubTexture,
    TextureErrorExistingTextureWrongType,
} TextureError;

namespace BGE {
    enum class TextureState {
        Unititialized,
        Loading,
        Loaded,
        Error
    };
    
    enum class TextureAlphaState {
        None,
        NonPremultiplied,
        Premultiplied
    };
    
    enum class TextureFormat {
        Undefined,
        Alpha,
        RGB565,
        RGB888,
        RGBA5551,
        RGBA4444,
        RGBA8888
    };
    
    
    class TextureBase : public NamedObject
    {
    public:
        static const std::string ErrorDomain;
        
        TextureBase(ObjectId texId, std::string name);
        TextureBase() = delete;
        TextureBase(TextureBase &) = delete;
        virtual ~TextureBase() {}
        
        TextureAlphaState getAlphaState() const { return alphaState_; }
        TextureFormat getFormat() const { return format_; }
        
        bool isValid() const { return valid_; }
        
        virtual uint32_t getHWTextureId() const =0;
        
        virtual uint32_t getWidth() const { return width_; }
        virtual uint32_t getHeight() const { return height_; }
        
        const Vector2 *getXYs() const { return &xys_[0]; }
        
        float getX0() const { return xys_[0].x; }
        float getY0() const { return xys_[0].y; }
        float getX1() const { return xys_[1].x; }
        float getY1() const { return xys_[1].y; }
        float getX2() const { return xys_[2].x; }
        float getY2() const { return xys_[2].y; }
        float getX3() const { return xys_[3].x; }
        float getY4() const { return xys_[3].y; }
        
        const Vector2 *getUVs() const { return &uvs_[0]; }
        
        float getU0() const { return uvs_[0].x; }
        float getV0() const { return uvs_[0].y; }
        float getU1() const { return uvs_[1].x; }
        float getV1() const { return uvs_[1].y; }
        float getU2() const { return uvs_[2].x; }
        float getV2() const { return uvs_[2].y; }
        float getU3() const { return uvs_[3].x; }
        float getV4() const { return uvs_[3].y; }
        
    protected:
        bool                    valid_;
        TextureFormat        format_;
        TextureAlphaState    alphaState_;
        
        uint32_t                x_;
        uint32_t                y_;
        uint32_t                width_;
        uint32_t                height_;
        
        Vector2              xys_[4];
        Vector2              uvs_[4];
        
        virtual void releaseCurrentTexture();
        
        void setValid(bool valid) { valid_ = valid; }
        void setFormat(TextureFormat format) { format_ = format; }
        void setAlphaState(TextureAlphaState alphaState) { alphaState_ = alphaState; }
        void setX(uint32_t x) { x_ = x; }
        void setY(uint32_t y) { y_ = y; }
        void setWidth(uint32_t width) { width_ = width; }
        void setHeight(uint32_t height) { height_ = height; }
        
        virtual void updateUVs();
        virtual void updateXYs();
    };
}

#endif /* BGETextureBase_h */
