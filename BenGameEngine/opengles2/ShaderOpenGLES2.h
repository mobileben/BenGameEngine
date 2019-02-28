//
//  ShaderOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#ifndef BGEShaderOpenGLES2_h
#define BGEShaderOpenGLES2_h

#include <stdio.h>
#include "Shader.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace BGE {
    class ShaderOpenGLES2 : public Shader
    {
    public:
        ShaderOpenGLES2(ShaderType shaderType, ShaderId id, std::string name);
        
        GLuint getShader() { return shader_; }
        
    private:
        GLuint shader_;
        
        GLenum error_;
        std::string errorString_;
    };
}

#endif /* BGEShaderOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
