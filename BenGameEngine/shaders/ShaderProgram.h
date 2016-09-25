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
        Unitialized = 0,
        GLError,
        IllegalShader,
        LinkError,
        Ready
    };
    
    class ShaderProgram
    {
    public:
        ShaderProgram(std::string name, std::vector<std::shared_ptr<Shader>> shaders);
        ShaderProgram(std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms);
        virtual ~ShaderProgram() {}
        
        std::string getName() { return name_; }
        
    protected:
        std::string name_;
        
        std::vector<std::shared_ptr<Shader>> shaders_;
        
        ShaderProgramState state_;
        
    private:
    };
}

#endif /* BGEShaderProgram_h */
