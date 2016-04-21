//
//  Shader.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEShader_h
#define BGEShader_h

#include <stdio.h>
#include <string>

namespace BGE {
    enum class ShaderType {
        Vertex = 0,
        Fragment
    };
    
    enum class ShaderState {
        Unitialized = 0,
        UnsupportedType,
        FileError,
        GLError,
        CompileError,
        Ready
    };
    
    class Shader
    {
    public:
        Shader(ShaderType shaderType, std::string name);
        virtual ~Shader() {}
        
        ShaderType getType() { return shaderType_; }
        std::string getName() { return name_; }
        ShaderState getState() { return state_; }
        
    protected:
        std::string name_;
        ShaderType shaderType_;
        ShaderState state_;
    };
}

#endif /* BGEShader_h */
