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
#include "TextureBase.h"

namespace BGE {
    class MaterialService;
    
    class Material : public Object {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<Material> create(ObjectId matId);
        
        Material(struct private_key const& key, ObjectId matId);
        ~Material() {}
        
        void getColor(Color& color) const { color = colorMatrix_.offset; }
        void setColor(Color& color) { colorMatrix_.offset = color; }
        void getColorMatrix(ColorMatrix& colorMatrix) const;
        void setColorMatrix(ColorMatrix& colorMatrix);
        
        const ColorMatrix *getColorMatrixRaw() const { return &colorMatrix_; }
        std::weak_ptr<TextureBase> getTexture() const { return texture_; }
        void setTexture(std::shared_ptr<TextureBase> texture) { texture_ = texture; }
        
    protected:
        friend MaterialService;
        
    private:
        bool                        colorDirty_;
        ColorMatrix                 colorMatrix_;
        
        std::weak_ptr<TextureBase>   texture_;
    };
}

#endif /* BGEMaterial_h */
