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
#include "MathTypes.h"
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
        
        const Matrix4 *getProjectionMatrix() { return &projectionMatrix_; }
        
        void drawRect(Vector2 &position, Vector2 &size, Vector4 &color);
        void drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4]);
        void drawTexture(Vector2 &position, std::shared_ptr<TextureBase> texture);
        
        // Using the more updated means
        void drawFlatRect(std::shared_ptr<BGE::GameObject> gameObject);
        // TODO: Transform
        void drawLines(const std::vector<Vector2>& points, float thickness, bool loop, std::shared_ptr<BGE::Material> material);
        void drawSprite(std::shared_ptr<BGE::GameObject> gameObject);
        
        int8_t createMask(Vector2 &position, std::shared_ptr<TextureBase> mask);
        void enableMask(int8_t maskId);
        void disableMask(int8_t maskId);
        
        void drawFont(Vector2 &position, std::shared_ptr<TextureBase> texture);
        void render();
        
        void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
        
    private:
        std::vector<Matrix4> matrixStack_;
        std::vector<ColorMatrix> colorMatrixStack_;
        
        bool texturingEnabled_[MaxTextureUnits];
        uint64_t currentTexture_[MaxTextureUnits];
        
        Matrix4 projectionMatrix_;
        GLKTextureInfo *textureInfo_;
        int8_t masksInUse_;
        uint8_t activeMasks_;
        
        void queueRender();
    };
}

#endif /* RenderServiceOpenGLES2_h */
