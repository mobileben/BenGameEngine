//
//  BGEShaderServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGEShaderServiceOpenGLES2_h
#define BGEShaderServiceOpenGLES2_h

#include <stdio.h>
#include "BGEShaderService.h"
#include "BGEShaderProgramOpenGLES2.h"
#include <GLKit/GLKit.h>

class BGEShaderServiceOpenGLES2 : public BGEShaderService
{
public:
    static void mapShaderBundle(std::string bundleName);
    static NSBundle *getShaderBundle() { return BGEShaderServiceOpenGLES2::shaderBundle_; }
    
    BGEShaderServiceOpenGLES2();
    
    std::shared_ptr<BGEShader> createShader(BGEShaderType shaderType, std::string name);
    std::shared_ptr<BGEShaderProgram> createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders);
    std::shared_ptr<BGEShaderProgram> createShaderProgram(std::string name, std::vector<std::shared_ptr<BGEShader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms);

private:
    static NSBundle *shaderBundle_;    
};

#endif /* BGEShaderServiceOpenGLES2_h */
