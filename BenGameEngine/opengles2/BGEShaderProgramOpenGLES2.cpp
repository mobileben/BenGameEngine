//
//  BGEShaderProgramOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include <Foundation/Foundation.h>
#include "BGEShaderProgramOpenGLES2.h"

BGEShaderProgramOpenGLES2::BGEShaderProgramOpenGLES2(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders) : BGEShaderProgram(name, shaders) {
    state_ = createShaderProgram(name, shaders);
}

BGEShaderProgramOpenGLES2::BGEShaderProgramOpenGLES2(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms) : BGEShaderProgram(name, shaders)
{
    state_ = createShaderProgram(name, shaders);
    
    
    if (state_ == BGEShaderProgramState::Ready) {
        glUseProgram(program_);

        createAttributesAndUniforms(attributes, uniforms);
    }
}

BGEShaderProgramState BGEShaderProgramOpenGLES2::createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders)
{
    program_ = glCreateProgram();
    
    if (program_ != 0) {
        for (std::shared_ptr<BGEShader> shader: shaders) {
            std::shared_ptr<BGEShaderOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderOpenGLES2>(shader);
            
            if (glShader) {
                glAttachShader(program_, glShader->getShader());
            } else {
                state_ = BGEShaderProgramState::IllegalShader;
                break;
            }
        }
        
        if (state_ == BGEShaderProgramState::Unitialized) {
            glLinkProgram(program_);
            
            GLint linkSuccess;
            
            glGetProgramiv(program_, GL_LINK_STATUS, &linkSuccess);
            
            if (linkSuccess == GL_TRUE) {
                state_ = BGEShaderProgramState::Ready;
            } else {
                GLchar message[256];
                // Terminate, just in case
                message[0] = '\0';
                glGetProgramInfoLog(program_, sizeof(message), 0, &message[0]);
                
                errorString_ = std::string(message);
                state_ = BGEShaderProgramState::LinkError;
            }
        }
    } else {
        state_ = BGEShaderProgramState::GLError;
    }
    
    return state_;
}

void BGEShaderProgramOpenGLES2::createAttributesAndUniforms(std::vector<std::string> attributes, std::vector<std::string> uniforms)
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

