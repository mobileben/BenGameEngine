//
//  ShaderProgram.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEShaderProgram_h
#define BGEShaderProgram_h

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "Shader.h"

namespace BGE {
    enum class ShaderProgramState {
        Uninitialized = 0,
        GLError,
        IllegalShader,
        LinkError,
        Ready
    };
    
    using ShaderProgramId = int32_t;
    const ShaderProgramId ShaderProgramIdUndefined = -1;

    using ShaderAttributeId = int32_t;
    using ShaderUniformId = int32_t;
    
    const ShaderAttributeId ShaderAttributeIdUndefined = -1;
    const ShaderUniformId ShaderUniformIdUndefined = -1;
    
    class ShaderProgram
    {
    public:
        ShaderProgram() : id_(ShaderProgramIdUndefined), state_(ShaderProgramState::Uninitialized) {}
        ShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders);
        ShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms);
        virtual ~ShaderProgram() {}
        
        inline ShaderProgramId getId() const { return id_; }
        inline std::string getName() const { return name_; }
        
    protected:
        ShaderProgramId id_;
        std::string     name_;
        
        std::vector<std::shared_ptr<Shader>> shaders_;
        
        ShaderProgramState state_;
        
    private:
    };
}

#endif /* BGEShaderProgram_h */
