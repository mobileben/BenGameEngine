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
        Uninitialized = 0,
        UnsupportedType,
        FileError,
        GLError,
        CompileError,
        Ready
    };
    
    using ShaderId = int32_t;
    const ShaderId ShaderIdUndefined = -1;
    
    class Shader
    {
    public:
        Shader() : id_(ShaderIdUndefined), shaderType_(ShaderType::Vertex), state_(ShaderState::Uninitialized) {}
        Shader(ShaderType shaderType, ShaderId id, std::string name);
        virtual ~Shader() {}
        
        inline ShaderId getId() const { return id_; }
        inline std::string getName() const { return name_; }
        inline ShaderType getType() const { return shaderType_; }
        inline ShaderState getState() const { return state_; }
        
    protected:
        ShaderId    id_;
        std::string name_;
        ShaderType  shaderType_;
        ShaderState state_;
    };
}

#endif /* BGEShader_h */
