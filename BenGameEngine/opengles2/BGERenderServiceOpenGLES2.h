//
//  BGERenderServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderServiceOpenGLES2_h
#define BGERenderServiceOpenGLES2_h

#include <stdio.h>
#include "BGERenderService.h"
#include "BGEShaderServiceOpenGLES2.h"
#include "BGEMathTypes.h"
#include "BGETexture.h"

// TODO: REmove
#include <GLKit/GLKit.h>

class BGERenderServiceOpenGLES2 : public BGERenderService
{
public:
    BGERenderServiceOpenGLES2();
    
    virtual void initialize();
    virtual void reset();
    virtual void enteringBackground();
    virtual void enteringForeground();
    virtual void pause();
    virtual void resume();
    virtual void destroy();

    static int8_t MaxActiveMasks;
    
    void setCoordinateSystem2D(BGERender2DCoordinateSystem coordSystem2D);

    void bindRenderWindow(std::shared_ptr<BGERenderContext> context, std::shared_ptr<BGERenderWindow> window);
    void resizeRenderWindow();
    void createShaders();

    std::shared_ptr<BGEShaderProgram> pushShaderProgram(std::string program);
    std::shared_ptr<BGEShaderProgram> popShaderProgram();
    
    const BGEMatrix4 *getProjectionMatrix() { return &projectionMatrix_; }
    
    void drawRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 &color);
    void drawShadedRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 color[4]);
    void drawTexture(BGEVector2 &position, std::shared_ptr<BGETextureBase> texture);
    int8_t createMask(BGEVector2 &position, std::shared_ptr<BGETextureBase> mask);
    void enableMask(int8_t maskId);
    void disableMask(int8_t maskId);
    
    void drawFont(BGEVector2 &position, std::shared_ptr<BGETextureBase> texture);
    void render();
    
    void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
    
private:
    BGEMatrix4 projectionMatrix_;
    GLKTextureInfo *textureInfo_;
    int8_t masksInUse_;
    uint8_t activeMasks_;
    
    void queueRender();
};

#endif /* BGERenderServiceOpenGLES2_h */
