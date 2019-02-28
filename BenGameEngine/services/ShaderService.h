//
//  ShaderService.h
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
#include "ShaderProgram.h"

namespace BGE {
    class ShaderService : public Service
    {
    public:
        
        virtual void initialize() {}
        virtual void reset() {}
        virtual void platformSuspending() { Service::platformSuspending(); }
        virtual void platformResuming() { Service::platformResuming(); }
        virtual void pause() { Service::pause(); }
        virtual void resume() { Service::resume(); }
        virtual void destroy() {}
        void update(__attribute__ ((unused)) double deltaTime) {}

        virtual std::shared_ptr<Shader> createShader(ShaderType shaderType, ShaderId id, std::string name) =0;
        virtual std::shared_ptr<ShaderProgram> createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders) =0;
        virtual std::shared_ptr<ShaderProgram> createShaderProgram(ShaderProgramId id, std::string name, std::vector<std::shared_ptr<Shader>> shaders, std::vector<std::string> attributes, std::vector<std::string> uniforms) =0;
        
        std::shared_ptr<Shader> getShader(ShaderId id);
        std::shared_ptr<Shader> getShader(std::string name);
        std::shared_ptr<ShaderProgram> getShaderProgram(ShaderProgramId id);
        std::shared_ptr<ShaderProgram> getShaderProgram(std::string name);
        
        void removeShader(ShaderId id);
        void removeShader(std::string name);
        void removeProgram(ShaderProgramId id);
        void removeProgram(std::string name);
        
    protected:
        // Vector variants
        std::vector<std::shared_ptr<Shader>>        shaderVector_;
        std::vector<std::shared_ptr<ShaderProgram>> shaderProgramVector_;

        // Map variants
        // TODO: Perhaps remove
        std::map<std::string, std::shared_ptr<Shader>> shaders_;
        std::map<std::string, std::shared_ptr<ShaderProgram>> shaderPrograms_;
    };
}

#endif /* BGEShaderService_h */
