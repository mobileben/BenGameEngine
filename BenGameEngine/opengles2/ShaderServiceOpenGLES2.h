//
//  ShaderServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#ifndef BGEShaderServiceOpenGLES2_h
#define BGEShaderServiceOpenGLES2_h

#include <stdio.h>
#include "ShaderService.h"
#include "ShaderProgramOpenGLES2.h"
#include <GLKit/GLKit.h>

namespace BGE {    
    class ShaderServiceOpenGLES2 : public ShaderService
    {
    public:
        static void mapShaderBundle(std::string bundleName);
        static NSBundle *getShaderBundle() { return ShaderServiceOpenGLES2::shaderBundle_; }
        
        ShaderServiceOpenGLES2();
        
        std::shared_ptr<Shader> createShader(ShaderType shaderType, ShaderId id, std::string name);
        std::shared_ptr<ShaderProgram> createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders);
        std::shared_ptr<ShaderProgram> createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::pair<ShaderVertexAttributeIndex, std::string>> attributes, std::vector<std::pair<ShaderUniformId, std::string>> uniforms, std::function<void(ShaderProgram *program)> firstUseFunction, std::function<void(ShaderProgram *program)> windowMappedDimensionsUpdated, std::function<void(ShaderProgram *program)> shaderChangedSetup);
        
    private:
        static NSBundle *shaderBundle_;    
    };
}

#endif /* BGEShaderServiceOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
