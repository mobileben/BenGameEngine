//
//  ShaderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include <assert.h>
#include "ShaderService.h"

std::shared_ptr<BGE::Shader> BGE::ShaderService::getShader(ShaderId id)
{
    for (auto s : shaderVector_) {
        if (s->getId() == id) {
            return s;
        }
    }
    return nullptr;
}

std::shared_ptr<BGE::Shader> BGE::ShaderService::getShader(std::string name)
{
    return shaders_[name];
}

std::shared_ptr<BGE::ShaderProgram> BGE::ShaderService::getShaderProgram(ShaderProgramId id)
{
    for (auto p : shaderProgramVector_) {
        if (p->getId() == id) {
            return p;
        }
    }
    return nullptr;
}

std::shared_ptr<BGE::ShaderProgram> BGE::ShaderService::getShaderProgram(std::string name)
{
    return shaderPrograms_[name];
}

void BGE::ShaderService::removeShader(ShaderId id)
{
    for (auto it=shaderVector_.begin();it!=shaderVector_.end();++it) {
        if ((*it)->getId() == id) {
            shaderVector_.erase(it);
            return;
        }
    }
}

void BGE::ShaderService::removeShader(std::string name)
{
    shaders_.erase(name);
}

void BGE::ShaderService::removeProgram(ShaderProgramId id)
{
    for (auto it=shaderProgramVector_.begin();it!=shaderProgramVector_.end();++it) {
        if ((*it)->getId() == id) {
            shaderProgramVector_.erase(it);
            return;
        }
    }
}

void BGE::ShaderService::removeProgram(std::string name)
{
    shaderPrograms_.erase(name);
}

void BGE::ShaderService::windowMappedDimensionsUpdated() {
    for (auto program : shaderProgramVector_) {
        program->windowMappedDimensionsUpdated();
    }
}
