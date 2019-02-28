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
        ShaderProgramOpenGLES2(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms);
        
        GLuint getProgram() const { return program_; }
        
        std::map<std::string, GLint> getAttributes() const { return attributes_; }
        
        GLint locationForAttribute(std::string attribute) { return attributes_[attribute]; }
        GLint locationForUniform(std::string uniform) { return uniforms_[uniform]; }
        
    private:
        GLuint program_;
        std::map<std::string, GLint> attributes_;
        std::map<std::string, GLint> uniforms_;
        
        GLenum error_;
        std::string errorString_;
        
        ShaderProgramState createShaderProgram(std::string name, std::vector<std::shared_ptr<Shader>> shaders);
        void createAttributesAndUniforms(std::vector<std::string> attributes, std::vector<std::string> uniforms);
    };
}

#endif /* BGEShaderProgramOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
