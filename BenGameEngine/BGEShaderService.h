//
//  BGEShaderService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEShaderService_h
#define BGEShaderService_h

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include "Service.h"
#include "BGEShaderProgram.h"

class BGEShaderService : public BGE::Service
{
public:
    
    virtual void initialize() {}
    virtual void reset() {}
    virtual void enteringBackground() {}
    virtual void enteringForeground() {}
    virtual void pause() {}
    virtual void resume() {}
    virtual void destroy() {}

    virtual std::shared_ptr<BGEShader> createShader(BGEShaderType shaderType, std::string name) =0;
    virtual std::shared_ptr<BGEShaderProgram> createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders) =0;
    virtual std::shared_ptr<BGEShaderProgram> createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms) =0;
    
    std::shared_ptr<BGEShader> getShader(std::string name);
    std::shared_ptr<BGEShaderProgram> getShaderProgram(std::string name);
    
    void removeShader(std::string name);
    void removeProgram(std::string name);
    
protected:
    std::map<std::string, std::shared_ptr<BGEShader>> shaders_;
    std::map<std::string, std::shared_ptr<BGEShaderProgram>> shaderPrograms_;
};

#endif /* BGEShaderService_h */
