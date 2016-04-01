//
//  BGEShader.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEShader_h
#define BGEShader_h

#include <stdio.h>
#include <string>

enum class BGEShaderType {
    Vertex = 0,
    Fragment
};

enum class BGEShaderState {
    Unitialized = 0,
    UnsupportedType,
    FileError,
    GLError,
    CompileError,
    Ready
};

class BGEShader
{
public:
    BGEShader(BGEShaderType shaderType, std::string name);
    virtual ~BGEShader() {}
    
    BGEShaderType getType() { return shaderType_; }
    std::string getName() { return name_; }
    BGEShaderState getState() { return state_; }

protected:
    std::string name_;
    BGEShaderType shaderType_;
    BGEShaderState state_;
};


#endif /* BGEShader_h */
