//
//  Material.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Material.h"

BGE::Material::Material(ObjectId matId) : Object(matId), colorDirty_(false)  {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

BGE::Material::Material() : Object(), colorDirty_(false)  {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

void BGE::Material::initialize(MaterialHandle handle) {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
    colorDirty_ = false;
    
    handle_ = handle;
}

void BGE::Material::destroy() {
    handle_ = MaterialHandle();
}

void BGE::Material::getColorMatrix(ColorMatrix& colorMatrix) const {
    memcpy(colorMatrix.c, colorMatrix_.c, sizeof(ColorMatrix));
}

void BGE::Material::setColorMatrix(ColorMatrix& colorMatrix) {
    memcpy(colorMatrix_.c, colorMatrix.c, sizeof(ColorMatrix));
}
