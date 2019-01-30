//
//  ShaderProgram.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ShaderProgram.h"

BGE::ShaderProgram::ShaderProgram(std::string name, __attribute__ ((unused)) std::vector<std::shared_ptr<Shader>> shaders) : name_(name), state_(ShaderProgramState::Unitialized)
{
}
