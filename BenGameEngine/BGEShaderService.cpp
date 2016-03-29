//
//  BGEShaderService.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include <assert.h>
#include "BGEShaderService.h"


std::shared_ptr<BGEShader> BGEShaderService::getShader(std::string name)
{
    return shaders_[name];
}

std::shared_ptr<BGEShaderProgram> BGEShaderService::getShaderProgram(std::string name)
{
    return shaderPrograms_[name];
}

void BGEShaderService::removeShader(std::string name)
{
    shaders_.erase(name);
}

void BGEShaderService::removeProgram(std::string name)
{
    shaderPrograms_.erase(name);
}
