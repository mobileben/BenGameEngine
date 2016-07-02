//
//  ShaderOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include <Foundation/Foundation.h>
#include "ShaderOpenGLES2.h"
#include "ShaderServiceOpenGLES2.h"

BGE::ShaderOpenGLES2::ShaderOpenGLES2(ShaderType shaderType, std::string name) : Shader(shaderType, name), shader_(0), error_(GL_NO_ERROR)
{
    NSString *path = [ShaderServiceOpenGLES2::getShaderBundle() pathForResource:[[NSString alloc] initWithCString:name.c_str() encoding:NSUTF8StringEncoding] ofType:@"glsl"];
    NSError *error = nil;
    NSString *shaderString = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
    
    if (shaderString.length) {
        const char *shaderStringUTF8 = [shaderString UTF8String];
        int shaderStringLength = (int)shaderString.length;
        
        // 0 is a non assigned shader value
        GLenum shader = 0;
        
        switch (shaderType) {
            case ShaderType::Vertex:
                shader = GL_VERTEX_SHADER;
                break;
            case ShaderType::Fragment:
                shader = GL_FRAGMENT_SHADER;
                break;
        }
        
        if (shader != 0) {
            shader_ = glCreateShader(shader);
            
            if (shader_ != 0) {
                glShaderSource(shader_, 1, &shaderStringUTF8, &shaderStringLength);
                
                glCompileShader(shader_);
                
                GLint compileSuccess;
                
                glGetShaderiv(shader_, GL_COMPILE_STATUS, &compileSuccess);
                
                if (compileSuccess == GL_TRUE) {
                    state_ = ShaderState::Ready;
                } else {
                    GLchar message[256];
                    // Terminate, just in case
                    message[0] = '\0';
                    glGetShaderInfoLog(shader_, sizeof(message), 0, &message[0]);
                    
                    errorString_ = std::string(message);
                    state_ = ShaderState::CompileError;
                }
            } else {
                state_  = ShaderState::GLError;
                error_ = glGetError();
            }
        } else {
            state_ = ShaderState::UnsupportedType;
        }
    } else {
        state_ = ShaderState::FileError;
    }
}