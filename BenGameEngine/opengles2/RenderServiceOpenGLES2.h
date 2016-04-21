//
//  RenderServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef RenderServiceOpenGLES2_h
#define RenderServiceOpenGLES2_h

#include <stdio.h>
#include "RenderService.h"
#include "ShaderServiceOpenGLES2.h"
#include "BGEMathTypes.h"
#include "Texture.h"
#include "Material.h"
#include "GameObject.h"

// TODO: REmove
#include <GLKit/GLKit.h>

namespace BGE {
    class RenderServiceOpenGLES2 : public RenderService
    {
    public:
        static const uint32_t MaxTextureUnits = 4;
        
        RenderServiceOpenGLES2();
        
        virtual void initialize();
        virtual void reset();
        virtual void enteringBackground();
        virtual void enteringForeground();
        virtual void pause();
        virtual void resume();
        virtual void destroy();
        
        static int8_t MaxActiveMasks;
        
        void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        void bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window);
        void resizeRenderWindow();
        void createShaders();
        
        std::shared_ptr<ShaderProgram> pushShaderProgram(std::string program);
        std::shared_ptr<ShaderProgram> popShaderProgram();
        
        const BGEMatrix4 *getProjectionMatrix() { return &projectionMatrix_; }
        
        void drawRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 &color);
        void drawShadedRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 color[4]);
        void drawTexture(BGEVector2 &position, std::shared_ptr<TextureBase> texture);
        
        // Using the more updated means
        void drawFlatRect(std::shared_ptr<BGE::GameObject> gameObject);
        // TODO: Transform
        void drawLines(const std::vector<BGEVector2>& points, float thickness, bool loop, std::shared_ptr<BGE::Material> material);
        void drawSprite(std::shared_ptr<BGE::GameObject> gameObject);
        
        int8_t createMask(BGEVector2 &position, std::shared_ptr<TextureBase> mask);
        void enableMask(int8_t maskId);
        void disableMask(int8_t maskId);
        
        void drawFont(BGEVector2 &position, std::shared_ptr<TextureBase> texture);
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
}

#endif /* RenderServiceOpenGLES2_h */
