//
//  ShaderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#include "ShaderServiceOpenGLES2.h"

NSBundle *BGE::ShaderServiceOpenGLES2::shaderBundle_ = nil;

void BGE::ShaderServiceOpenGLES2::mapShaderBundle(std::string bundleName) {
    if (!ShaderServiceOpenGLES2::shaderBundle_) {
        ShaderServiceOpenGLES2::shaderBundle_ = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:[[NSString alloc] initWithCString:bundleName.c_str() encoding:NSUTF8StringEncoding] withExtension:@"bundle"]];
    }
}

BGE::ShaderServiceOpenGLES2::ShaderServiceOpenGLES2()
{
}

std::shared_ptr<BGE::Shader> BGE::ShaderServiceOpenGLES2::createShader(ShaderType shaderType, ShaderId id, std::string name)
{
    assert(ShaderServiceOpenGLES2::shaderBundle_);
    
    std::shared_ptr<ShaderOpenGLES2> shader = std::dynamic_pointer_cast<ShaderOpenGLES2>(shaders_[name]);
    
    if (!shader) {
        shader = std::make_shared<ShaderOpenGLES2>(shaderType, id, name);
        
        if (shader) {
            // Check if id already exists, if so throw assert
            auto existing = getShader(id);
            assert(!existing);
            
            // Check if name already exists, if so throw assert
            existing = getShader(name);
            assert(!existing);
            
            shaderVector_.push_back(shader);
            shaders_[shader->getName()] = shader;
        }
    }
    
    return shader;
}

std::shared_ptr<BGE::ShaderProgram> BGE::ShaderServiceOpenGLES2::createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders)
{
    std::shared_ptr<ShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderPrograms_[name]);
    
    if (!program) {
        program = std::make_shared<ShaderProgramOpenGLES2>(id, name, shaders);
        
        if (program) {
            // Check if id already exists, if so throw assert
            auto existing = getShaderProgram(id);
            assert(!existing);
            
            // Check if name already exists, if so throw assert
            existing = getShaderProgram(name);
            assert(!existing);
            
            shaderProgramVector_.push_back(program);
            shaderPrograms_[program->getName()] = program;
        }
    }
    
    return program;
}

std::shared_ptr<BGE::ShaderProgram> BGE::ShaderServiceOpenGLES2::createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms)
{
    std::shared_ptr<ShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderPrograms_[name]);

    if (!program) {
        program = std::make_shared<ShaderProgramOpenGLES2>(id, name, shaders, attributes, uniforms);
        
        if (program) {
            // Check if id already exists, if so throw assert
            auto existing = getShaderProgram(id);
            assert(!existing);
            
            // Check if name already exists, if so throw assert
            existing = getShaderProgram(name);
            assert(!existing);
            
            shaderProgramVector_.push_back(program);
            shaderPrograms_[program->getName()] = program;
        }
    }
    
    return program;
}

#endif /* SUPPORT_OPENGLES2 */

