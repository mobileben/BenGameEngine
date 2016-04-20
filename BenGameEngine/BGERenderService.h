//
//  BGERenderService.h
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
#include "BGERenderWindow.h"
#include "BGERenderContext.h"
#include "BGEShaderService.h"
#include "BGEMathTypes.h"

typedef BGEVector3 BGEVertex;

typedef struct {
    BGEVector3 position;
    BGEVector4 color;
} BGEVertexColor;

typedef struct {
    BGEVector3 position;
    BGEVector2 tex;
} BGEVertexTex;

typedef struct {
    BGEVector3 position;
    BGEVector4 color;
    BGEVector2 tex;
} BGEVertexColorTex;

typedef struct {
    BGEVector3 position;
    BGEVector4 color;
    BGEVector2 tex;
    BGEVector2 normal;
} BGEVertexColorTexNormal;

enum class BGERender2DCoordinateSystem {
    Traditional,            // Upper left (X-right, Y-down)
    TraditionalCentered,    // Center of screen (X-right, Y-down)
    OpenGL,                 // Lower left (X-right, Y-up)
    OpenGLCentered          // Center of screen (X-right, Y-up)
};

class BGERenderService: public BGE::Service
{
public:
    BGERenderService();
    
    virtual void bindRenderWindow(std::shared_ptr<BGERenderContext> context, std::shared_ptr<BGERenderWindow> window);
    virtual void resizeRenderWindow();
    virtual void createShaders();
    
    bool isReady() const { return ready_; }
    void setIsReady() { ready_ = true; }
    
    BGERender2DCoordinateSystem getCoordinateSystem2D() const { return coordSystem2D_; }
    
    // Y-up is inverted, Y-down is normal
    bool hasInvertedYAxis() const { return invertedYAxis_; }
    
    void setCoordinateSystem2D(BGERender2DCoordinateSystem coordSystem2D);
    
    std::shared_ptr<BGERenderContext> getRenderContext() { return renderContext_; }
    
    std::shared_ptr<BGERenderWindow> getRenderWindow() { return renderWindow_; }
    
    std::shared_ptr<BGEShaderService> getShaderService() { return shaderService_; }
    
    virtual std::shared_ptr<BGEShaderProgram> pushShaderProgram(std::string program) =0;
    virtual std::shared_ptr<BGEShaderProgram> popShaderProgram() =0;
    
    virtual void render() =0;
    
protected:
    std::shared_ptr<BGERenderContext> renderContext_;
    std::shared_ptr<BGERenderWindow> renderWindow_;
    std::shared_ptr<BGEShaderService> shaderService_;
    
    std::vector<std::shared_ptr<BGEShaderProgram>> shaderProgramStack_;
    
private:
    bool ready_;
    bool invertedYAxis_;    // Y-up is inverted, Y-down is normal
    BGERender2DCoordinateSystem coordSystem2D_;
};

#endif /* BGERenderService_h */
