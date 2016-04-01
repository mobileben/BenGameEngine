//
//  BGEShaderOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEShaderOpenGLES2_h
#define BGEShaderOpenGLES2_h

#include <stdio.h>
#include "BGEShader.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

class BGEShaderOpenGLES2 : public BGEShader
{
public:
    BGEShaderOpenGLES2(BGEShaderType shaderType, std::string name);
    
    GLuint getShader() { return shader_; }
    
private:
    GLuint shader_;
    
    GLenum error_;
    std::string errorString_;
};

#endif /* BGEShaderOpenGLES2_h */
