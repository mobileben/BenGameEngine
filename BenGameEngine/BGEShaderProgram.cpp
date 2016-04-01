//
//  BGEShaderProgram.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEShaderProgram.h"

BGEShaderProgram::BGEShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders) : name_(name), state_(BGEShaderProgramState::Unitialized)
{
}
