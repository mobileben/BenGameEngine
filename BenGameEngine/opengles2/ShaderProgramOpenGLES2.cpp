//
//  ShaderProgramOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#include "ShaderProgramOpenGLES2.h"

BGE::ShaderProgramOpenGLES2::ShaderProgramOpenGLES2(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders) : ShaderProgram(id, name, shaders), error_(GL_NO_ERROR) {
    state_ = createShaderProgram(name, shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>>());
}

BGE::ShaderProgramOpenGLES2::ShaderProgramOpenGLES2(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes, std::vector<std::pair<ShaderUniformId, std::string>> uniforms, std::function<void(ShaderProgram *program)> firstUseFunction, std::function<void(ShaderProgram *program)> windowMappedDimensionsUpdated, std::function<void(ShaderProgram *program)> shaderChangedSetup) : ShaderProgram(id, name, shaders), error_(GL_NO_ERROR)
{
    state_ = createShaderProgram(name, shaders, attributes);
    windowMappedDimensionsUpdated_ = windowMappedDimensionsUpdated;
    shaderChangedSetup_ = shaderChangedSetup;
    
    if (state_ == ShaderProgramState::Ready) {
        glUseProgram(program_);

        createUniforms(uniforms);
        
        if (firstUseFunction) {
            firstUseFunction(this);
        }
        if (windowMappedDimensionsUpdated) {
            windowMappedDimensionsUpdated(this);
        }
    }
}

BGE::ShaderProgramState BGE::ShaderProgramOpenGLES2::createShaderProgram(__attribute__ ((unused)) std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes)
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
        
        if (state_ == ShaderProgramState::Uninitialized) {
            if (createAttributes(attributes) == GL_NO_ERROR) {
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
                    assert(false);
                }
            } else {
                state_ = ShaderProgramState::GLError;
                assert(false);
            }
        }
    } else {
        state_ = ShaderProgramState::GLError;
        assert(false);
    }
    
    return state_;
}

GLint BGE::ShaderProgramOpenGLES2::locationForUniform(ShaderUniformId uniform) {
    auto it = uniformsById_.find(uniform);
    if (it != uniformsById_.end()) {
        return it->second;
    } else {
        return 0;
    }
}

GLenum BGE::ShaderProgramOpenGLES2::createAttributes(std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes)
{
    for (auto& attr : attributes) {
        auto& key = attr.second;
        glBindAttribLocation(program_, attr.first, key.c_str());
        auto err = glGetError();
        if (err != GL_NO_ERROR) {
            return err;
        }
    }
    return GL_NO_ERROR;
}

void BGE::ShaderProgramOpenGLES2::createUniforms(std::vector<std::pair<ShaderUniformId, std::string>> uniforms)
{
    for (auto& uni : uniforms) {
        auto& key = uni.second;
        GLint location = glGetUniformLocation(program_, key.c_str());
        uniformsById_[uni.first] = location;
        uniforms_[key] = location;
    }
}

#endif /* SUPPORT_OPENGLES2 */

