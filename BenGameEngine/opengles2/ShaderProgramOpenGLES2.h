//
//  ShaderProgramOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#ifndef BGEShaderProgramOpenGLES2_h
#define BGEShaderProgramOpenGLES2_h

#include <stdio.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ShaderProgram.h"
#include "ShaderOpenGLES2.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace BGE {
    
    class ShaderProgramOpenGLES2 : public ShaderProgram
    {
    public:
        ShaderProgramOpenGLES2(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders);
        ShaderProgramOpenGLES2(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes, std::vector<std::pair<ShaderUniformId, std::string>> uniforms, std::function<void(ShaderProgram *program)> firstUseFunction, std::function<void(ShaderProgram *program)> windowMappedDimensionsUpdated, std::function<void(ShaderProgram *program)> shaderChangedSetup);
        
        GLuint getProgram() const { return program_; }
        
        GLint locationForUniform(ShaderUniformId uniform);
        GLint locationForUniform(std::string uniform) { return uniforms_[uniform]; }

    private:
        GLuint program_;
        std::map<ShaderUniformId, GLint> uniformsById_;
        std::map<std::string, GLint> uniforms_;
        
        GLenum error_;
        std::string errorString_;
        
        ShaderProgramState createShaderProgram(std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes);
        GLenum createAttributes(std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes);
        void createUniforms(std::vector<std::pair<ShaderUniformId, std::string>> uniforms);
    };
}

#endif /* BGEShaderProgramOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
