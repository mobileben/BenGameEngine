//
//  RenderService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderService_h
#define BGERenderService_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "Service.h"
#include "RenderWindow.h"
#include "RenderContext.h"
#include "ShaderService.h"
#include "MathTypes.h"

namespace BGE {
    typedef Vector3 Vertex;
    
    typedef struct {
        Vector3 position;
        Vector4 color;
    } VertexColor;
    
    typedef struct {
        Vector3 position;
        Vector2 tex;
    } VertexTex;
    
    typedef struct {
        Vector3 position;
        Vector4 color;
        Vector2 tex;
    } VertexColorTex;
    
    typedef struct {
        Vector3 position;
        Vector4 color;
        Vector2 tex;
        Vector2 normal;
    } VertexColorTexNormal;

    enum class Render2DCoordinateSystem {
        Traditional,            // Upper left (X-right, Y-down)
        TraditionalCentered,    // Center of screen (X-right, Y-down)
        OpenGL,                 // Lower left (X-right, Y-up)
        OpenGLCentered          // Center of screen (X-right, Y-up)
    };
    
    class RenderService: public Service
    {
    public:
        RenderService();
        
        virtual void bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window);
        virtual void resizeRenderWindow();
        virtual void createShaders();
        
        bool isReady() const { return ready_; }
        void setIsReady() { ready_ = true; }
        
        Render2DCoordinateSystem getCoordinateSystem2D() const { return coordSystem2D_; }
        
        // Y-up is inverted, Y-down is normal
        bool hasInvertedYAxis() const { return invertedYAxis_; }
        
        void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        std::shared_ptr<RenderContext> getRenderContext() { return renderContext_; }
        
        std::shared_ptr<RenderWindow> getRenderWindow() { return renderWindow_; }
        
        std::shared_ptr<ShaderService> getShaderService() { return shaderService_; }
        
        virtual std::shared_ptr<ShaderProgram> pushShaderProgram(std::string program) =0;
        virtual std::shared_ptr<ShaderProgram> popShaderProgram() =0;
        
        virtual void render() =0;
        
    protected:
        std::shared_ptr<RenderContext> renderContext_;
        std::shared_ptr<RenderWindow> renderWindow_;
        std::shared_ptr<ShaderService> shaderService_;
        
        std::vector<std::shared_ptr<ShaderProgram>> shaderProgramStack_;
        
    private:
        bool ready_;
        bool invertedYAxis_;    // Y-up is inverted, Y-down is normal
        Render2DCoordinateSystem coordSystem2D_;
    };
}

#endif /* BGERenderService_h */
