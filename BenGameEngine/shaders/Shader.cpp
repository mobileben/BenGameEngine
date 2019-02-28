//
//  Shader.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Shader.h"

BGE::Shader::Shader(ShaderType shaderType, ShaderId id, std::string name) : id_(id), name_(name), shaderType_(shaderType), state_(ShaderState::Uninitialized)
{
    assert(id != ShaderIdUndefined);
}
