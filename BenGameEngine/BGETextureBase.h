//
//  BGETextureBase.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/21/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGETextureBase_h
#define BGETextureBase_h

#include <stdio.h>
#include <string>

#include "BGEMathTypes.h"

enum class BGETextureState {
    Unititialized,
    Loading,
    Loaded,
    Error
};

enum class BGETextureAlphaState {
    None,
    NonPremultiplied,
    Premultiplied
};

enum class BGETextureFormat {
    Undefined,
    Alpha,
    RGB565,
    RGB888,
    RGBA5551,
    RGBA4444,
    RGBA8888
};

typedef enum {
    BGETextureErrorNone = 0,
    BGETextureErrorOS,
    BGETextureErrorNoBuffer,
    BGETextureErrorZeroWidthOrHeight,
    BGETextureErrorClassAllocation,
    BGETextureErrorAllocation,
    BGETextureErrorInvalidSubTexture,
    BGETextureErrorExistingTextureWrongType,
} BGETextureError;

class BGETextureBase : public  std::enable_shared_from_this<BGETextureBase>
{
public:
    static const std::string ErrorDomain;
    
    BGETextureBase(std::string name);
    virtual ~BGETextureBase() {}
    
    BGETextureAlphaState getAlphaState() const { return alphaState_; }
    BGETextureFormat getFormat() const { return format_; }
    
    bool isValid() const { return valid_; }
    std::string getName() const { return name_; }

    virtual uint32_t getHWTextureId() const =0;
    
    virtual uint32_t getWidth() const { return width_; }
    virtual uint32_t getHeight() const { return height_; }
    
    const BGEVector2 *getXYs() const { return &xys_[0]; }
    
    float getX0() const { return xys_[0].x; }
    float getY0() const { return xys_[0].y; }
    float getX1() const { return xys_[1].x; }
    float getY1() const { return xys_[1].y; }
    float getX2() const { return xys_[2].x; }
    float getY2() const { return xys_[2].y; }
    float getX3() const { return xys_[3].x; }
    float getY4() const { return xys_[3].y; }

    const BGEVector2 *getUVs() const { return &uvs_[0]; }
    
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
    BGETextureFormat        format_;
    BGETextureAlphaState    alphaState_;
    
    std::string             name_;
    
    uint32_t                x_;
    uint32_t                y_;
    uint32_t                width_;
    uint32_t                height_;
    
    BGEVector2              xys_[4];
    BGEVector2              uvs_[4];
    
    virtual void releaseCurrentTexture();
    
    void setValid(bool valid) { valid_ = valid; }
    void setFormat(BGETextureFormat format) { format_ = format; }
    void setAlphaState(BGETextureAlphaState alphaState) { alphaState_ = alphaState; }
    void setX(uint32_t x) { x_ = x; }
    void setY(uint32_t y) { y_ = y; }
    void setWidth(uint32_t width) { width_ = width; }
    void setHeight(uint32_t height) { height_ = height; }
    
    virtual void updateUVs();
    virtual void updateXYs();
};

#endif /* BGETextureBase_h */
