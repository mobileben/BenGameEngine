//
//  BGEShaderOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include <Foundation/Foundation.h>
#include "BGEShaderOpenGLES2.h"
#include "BGEShaderServiceOpenGLES2.h"

BGEShaderOpenGLES2::BGEShaderOpenGLES2(BGEShaderType shaderType, std::string name) : BGEShader(shaderType, name), shader_(0), error_(GL_NO_ERROR)
{
    NSString *path = [BGEShaderServiceOpenGLES2::getShaderBundle() pathForResource:[[NSString alloc] initWithCString:name.c_str() encoding:NSUTF8StringEncoding] ofType:@"glsl"];
    NSError *error = nil;
    NSString *shaderString = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
    
    if (shaderString.length) {
        const char *shaderStringUTF8 = [shaderString UTF8String];
        int shaderStringLength = (int)shaderString.length;
        
        // 0 is a non assigned shader value
        GLenum shader = 0;
        
        switch (shaderType) {
            case BGEShaderType::Vertex:
                shader = GL_VERTEX_SHADER;
                break;
            case BGEShaderType::Fragment:
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
                    state_ = BGEShaderState::Ready;
                } else {
                    GLchar message[256];
                    // Terminate, just in case
                    message[0] = '\0';
                    glGetShaderInfoLog(shader_, sizeof(message), 0, &message[0]);
                    
                    errorString_ = std::string(message);
                    state_ = BGEShaderState::CompileError;
                }
            } else {
                state_  = BGEShaderState::GLError;
                error_ = glGetError();
            }
        } else {
            state_ = BGEShaderState::UnsupportedType;
        }
    } else {
        state_ = BGEShaderState::FileError;
    }
}