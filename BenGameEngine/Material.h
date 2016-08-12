//
//  Material.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEMaterial_h
#define BGEMaterial_h

#include <stdio.h>
#include <memory>
#include <string>
#include "Object.h"
#include "MathTypes.h"
#include "Texture.h"
#include "Handle.h"

namespace BGE {
    class MaterialService;
    
    class Material : public Object {
    public:
        Material(ObjectId matId);
        ~Material() {}
        
        void initialize(MaterialHandle handle, ObjectId matId);
        
        MaterialHandle getHandle() const { return handle_; }
        
        void getColor(Color& color) const { color = colorMatrix_.offset; }
        void setColor(Color& color) { colorMatrix_.offset = color; }
        void getColorMatrix(ColorMatrix& colorMatrix) const;
        void setColorMatrix(ColorMatrix& colorMatrix);
        
        const ColorMatrix *getColorMatrixRaw() const { return &colorMatrix_; }
        TextureHandle getTextureHandle() const { return textureHandle_; }
        void setTextureHandle(TextureHandle textureHandle) { textureHandle_ = textureHandle; }
        
    protected:
        friend MaterialService;
        
    private:
        MaterialHandle              handle_;
        bool                        colorDirty_;
        ColorMatrix                 colorMatrix_;
        TextureHandle               textureHandle_;
    };
}

#endif /* BGEMaterial_h */
