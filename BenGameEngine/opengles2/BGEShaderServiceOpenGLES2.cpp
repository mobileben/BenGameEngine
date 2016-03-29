//
//  BGEShaderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGEShaderServiceOpenGLES2.h"

NSBundle *BGEShaderServiceOpenGLES2::shaderBundle_ = nil;

void BGEShaderServiceOpenGLES2::mapShaderBundle(std::string bundleName) {
    if (!BGEShaderServiceOpenGLES2::shaderBundle_) {
        BGEShaderServiceOpenGLES2::shaderBundle_ = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:[[NSString alloc] initWithCString:bundleName.c_str() encoding:NSUTF8StringEncoding] withExtension:@"bundle"]];
    }
}

BGEShaderServiceOpenGLES2::BGEShaderServiceOpenGLES2()
{
}

std::shared_ptr<BGEShader> BGEShaderServiceOpenGLES2::createShader(BGEShaderType shaderType, std::string name)
{
    assert(BGEShaderServiceOpenGLES2::shaderBundle_);
    
    std::shared_ptr<BGEShaderOpenGLES2> shader = std::dynamic_pointer_cast<BGEShaderOpenGLES2>(shaders_[name]);
    
    if (!shader) {
        shader = std::make_shared<BGEShaderOpenGLES2>(shaderType, name);
        
        if (shader) {
            shaders_[shader->getName()] = shader;
        }
    }
    
    return shader;
}

std::shared_ptr<BGEShaderProgram> BGEShaderServiceOpenGLES2::createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders)
{
    std::shared_ptr<BGEShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(shaderPrograms_[name]);
    
    if (!program) {
        program = std::make_shared<BGEShaderProgramOpenGLES2>(name, shaders);
        
        if (program) {
            shaderPrograms_[program->getName()] = program;
        }
    }
    
    return program;
}

std::shared_ptr<BGEShaderProgram> BGEShaderServiceOpenGLES2::createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms)
{
    std::shared_ptr<BGEShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(shaderPrograms_[name]);

    if (!program) {
        program = std::make_shared<BGEShaderProgramOpenGLES2>(name, shaders, attributes, uniforms);
        
        if (program) {
            shaderPrograms_[program->getName()] = program;
        }
    }
    
    return program;
}
