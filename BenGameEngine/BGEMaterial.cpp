//
//  BGEMaterial.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEMaterial.h"

BGEMaterial::BGEMaterial(uint64_t matId) : BGEObject(matId), colorDirty_(false), color_({ 0, 0, 0, 1 })  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGEMaterial::BGEMaterial(uint64_t matId, std::string name) : BGEObject(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }) {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGEMaterial::BGEMaterial(uint64_t matId, std::string name, BGEVector4& color) : BGEObject(matId, name), colorDirty_(false), color_(color)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGEMaterial::BGEMaterial(uint64_t matId, std::string name, std::shared_ptr<BGETextureBase> texture) : BGEObject(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }), texture_(texture)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGEMaterial::BGEMaterial(uint64_t matId, std::string name, BGEVector4& color, std::shared_ptr<BGETextureBase> texture) : BGEObject(matId, name), colorDirty_(false), color_(color), texture_(texture)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

void BGEMaterial::setColor(BGEColor& color) {
    color_ = color;
}

