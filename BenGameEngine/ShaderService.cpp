//
//  ShaderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include <assert.h>
#include "ShaderService.h"

std::shared_ptr<BGE::Shader> BGE::ShaderService::getShader(std::string name)
{
    return shaders_[name];
}

std::shared_ptr<BGE::ShaderProgram> BGE::ShaderService::getShaderProgram(std::string name)
{
    return shaderPrograms_[name];
}

void BGE::ShaderService::removeShader(std::string name)
{
    shaders_.erase(name);
}

void BGE::ShaderService::removeProgram(std::string name)
{
    shaderPrograms_.erase(name);
}
