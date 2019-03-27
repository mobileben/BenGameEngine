//
//  ShaderProgram.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ShaderProgram.h"

BGE::ShaderProgram::ShaderProgram(ShaderProgramId id, std::string name, __attribute__ ((unused)) std::vector<std::shared_ptr<Shader>> shaders) : id_(id), name_(name), state_(ShaderProgramState::Uninitialized), needsWindowMappedDimensionsUpdate_(true), windowMappedDimensionsUpdated_(nullptr)
{
    assert(id != ShaderProgramIdUndefined);
}

void BGE::ShaderProgram::windowMappedDimensionsUpdated() {
    needsWindowMappedDimensionsUpdate_ = true;
}

void BGE::ShaderProgram::shaderChangedSetup() {
    if (needsWindowMappedDimensionsUpdate_) {
        if (windowMappedDimensionsUpdated_) {
            windowMappedDimensionsUpdated_(this);
        }
        needsWindowMappedDimensionsUpdate_ = false;
    }
    if (shaderChangedSetup_) {
        shaderChangedSetup_(this);
    }
}
