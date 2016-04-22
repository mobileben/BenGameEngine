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

BGE::Material::Material(struct private_key const& key, uint64_t matId) : BGE::Object(matId), colorDirty_(false), color_({ 0, 0, 0, 1 })  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(struct private_key const& key, uint64_t matId, std::string name) : BGE::Object(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }) {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId) : BGE::Object(matId), colorDirty_(false), color_({ 0, 0, 0, 1 })  {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

BGE::Material::Material(uint64_t matId, std::string name) : BGE::Object(matId, name), colorDirty_(false), color_({ 0, 0, 0, 1 }) {
    BGEMatrix4MakeIdentify(colorMatrix_);
}

void BGE::Material::setColor(BGEColor& color) {
    color_ = color;
}

