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
#include "BGEMaterial.h"
#include "BGEGameObject.h"

// TODO: REmove
#include <GLKit/GLKit.h>

class BGERenderServiceOpenGLES2 : public BGERenderService
{
public:
    static const uint32_t MaxTextureUnits = 4;
    
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
    
    // Using the more updated means
    void drawFlatRect(std::shared_ptr<BGEGameObject> gameObject);
    // TODO: Transform
    void drawLines(const std::vector<BGEVector2>& points, float thickness, bool loop, std::shared_ptr<BGEMaterial> material);
    void drawSprite(std::shared_ptr<BGEGameObject> gameObject);
    
    int8_t createMask(BGEVector2 &position, std::shared_ptr<BGETextureBase> mask);
    void enableMask(int8_t maskId);
    void disableMask(int8_t maskId);
    
    void drawFont(BGEVector2 &position, std::shared_ptr<BGETextureBase> texture);
    void render();
    
    void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
    
private:
    std::vector<BGEMatrix4> matrixStack_;
    std::vector<BGEColorMatrix> colorMatrixStack_;
    
    bool texturingEnabled_[MaxTextureUnits];
    uint64_t currentTexture_[MaxTextureUnits];
    
    BGEMatrix4 projectionMatrix_;
    GLKTextureInfo *textureInfo_;
    int8_t masksInUse_;
    uint8_t activeMasks_;
    
    void queueRender();
};

#endif /* BGERenderServiceOpenGLES2_h */
