//
//  BGEShader.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGEShader.h"

BGEShader::BGEShader(BGEShaderType shaderType, std::string name) : name_(name), shaderType_(shaderType), state_(BGEShaderState::Unitialized)
{
}
