//
//  ShaderProgramOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#include "ShaderProgramOpenGLES2.h"

BGE::ShaderProgramOpenGLES2::ShaderProgramOpenGLES2(std::string name, std::vector<std::shared_ptr<Shader>> shaders) : ShaderProgram(name, shaders), error_(GL_NO_ERROR) {
    state_ = createShaderProgram(name, shaders);
}

BGE::ShaderProgramOpenGLES2::ShaderProgramOpenGLES2(std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms) : ShaderProgram(name, shaders), error_(GL_NO_ERROR)
{
    state_ = createShaderProgram(name, shaders);
    
    
    if (state_ == ShaderProgramState::Ready) {
        glUseProgram(program_);

        createAttributesAndUniforms(attributes, uniforms);
    }
}

BGE::ShaderProgramState BGE::ShaderProgramOpenGLES2::createShaderProgram(__attribute__ ((unused)) std::string name, std::vector<std::shared_ptr<Shader>> shaders)
{
    program_ = glCreateProgram();
    
    if (program_ != 0) {
        for (std::shared_ptr<Shader> shader: shaders) {
            std::shared_ptr<ShaderOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderOpenGLES2>(shader);
            
            if (glShader) {
                glAttachShader(program_, glShader->getShader());
            } else {
                state_ = ShaderProgramState::IllegalShader;
                break;
            }
        }
        
        if (state_ == ShaderProgramState::Unitialized) {
            glLinkProgram(program_);
            
            GLint linkSuccess;
            
            glGetProgramiv(program_, GL_LINK_STATUS, &linkSuccess);
            
            if (linkSuccess == GL_TRUE) {
                state_ = ShaderProgramState::Ready;
            } else {
                GLchar message[256];
                // Terminate, just in case
                message[0] = '\0';
                glGetProgramInfoLog(program_, sizeof(message), 0, &message[0]);
                
                error_ = glGetError();
                errorString_ = std::string(message);
                state_ = ShaderProgramState::LinkError;
            }
        }
    } else {
        state_ = ShaderProgramState::GLError;
        assert(false);
    }
    
    return state_;
}

void BGE::ShaderProgramOpenGLES2::createAttributesAndUniforms(std::vector<std::string> attributes, std::vector<std::string> uniforms)
{
    for (std::string key: attributes) {
        GLint location = glGetAttribLocation(program_, key.c_str());
        
        attributes_[key] = location;
    }
    
    for (std::string key: uniforms) {
        GLint location = glGetUniformLocation(program_, key.c_str());
        
        uniforms_[key] = location;
    }
}

#endif /* SUPPORT_OPENGLES2 */

