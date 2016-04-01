//
//  BGEShaderProgram.h
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
#include "BGEShader.h"


enum class BGEShaderProgramState {
    Unitialized = 0,
    GLError,
    IllegalShader,
    LinkError,
    Ready
};

class BGEShaderProgram
{
public:
    BGEShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders);
    BGEShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms);
    virtual ~BGEShaderProgram() {}
    
    std::string getName() { return name_; }
    
protected:
    std::string name_;
    
    std::vector<std::shared_ptr<BGEShader>> shaders_;
    
    BGEShaderProgramState state_;
    
private:
};

#endif /* BGEShaderProgram_h */
