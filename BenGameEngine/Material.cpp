//
//  Material.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Material.h"

std::shared_ptr<BGE::Material> BGE::Material::create(uint64_t matId) {
    return std::make_shared<Material>(private_key{}, matId);
}

std::shared_ptr<BGE::Material> BGE::Material::create(uint64_t matId, std::string name) {
    return std::make_shared<Material>(private_key{}, matId, name);
}

BGE::Material::Material(struct private_key const& key, uint64_t matId) : BGE::Object(matId), colorDirty_(false)  {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

BGE::Material::Material(struct private_key const& key, uint64_t matId, std::string name) : BGE::Object(matId, name), colorDirty_(false) {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

BGE::Material::Material(uint64_t matId) : BGE::Object(matId), colorDirty_(false) {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

BGE::Material::Material(uint64_t matId, std::string name) : BGE::Object(matId, name), colorDirty_(false) {
    Matrix4MakeIdentify(colorMatrix_.matrix);
    colorMatrix_.offset.r = 0;
    colorMatrix_.offset.g = 0;
    colorMatrix_.offset.b = 0;
    colorMatrix_.offset.a = 0;
}

void BGE::Material::getColorMatrix(ColorMatrix& colorMatrix) const {
    memcpy(colorMatrix.c, colorMatrix_.c, sizeof(ColorMatrix));
}

void BGE::Material::setColorMatrix(ColorMatrix& colorMatrix) {
    memcpy(colorMatrix_.c, colorMatrix.c, sizeof(ColorMatrix));
}
