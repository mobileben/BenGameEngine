//
//  Material.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Material.h"

BGE::Material::Material(uint64_t matId) : BGE::Object(matId), colorDirty_(false), color_({ 0, 0, 0, 1 })  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId, std::string name) : BGE::Object(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }) {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId, std::string name, BGEVector4& color) : BGE::Object(matId, name), colorDirty_(false), color_(color)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId, std::string name, std::shared_ptr<BGETextureBase> texture) : BGE::Object(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }), texture_(texture)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId, std::string name, BGEVector4& color, std::shared_ptr<BGETextureBase> texture) : BGE::Object(matId, name), colorDirty_(false), color_(color), texture_(texture)  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

void BGE::Material::setColor(BGEColor& color) {
    color_ = color;
}

