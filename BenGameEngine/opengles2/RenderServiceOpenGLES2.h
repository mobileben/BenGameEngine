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
    class TransformComponent;
    
    class RenderServiceOpenGLES2 : public RenderService
    {
    public:
        static const uint32_t MaxTextureUnits = 4;
        
        RenderServiceOpenGLES2();
        
        virtual void initialize() final;
        virtual void reset() final;
        virtual void enteringBackground() final;
        virtual void enteringForeground() final;
        virtual void pause() final;
        virtual void resume() final;
        virtual void destroy() final;
        void update(double deltaTime) final {}

        static int8_t MaxActiveMasks;
        
        size_t totalMemory() const final;

        void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        void bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window);
        void resizeRenderWindow();
        void createShaders();
        
        std::shared_ptr<ShaderProgram> pushShaderProgram(std::string program);
        std::shared_ptr<ShaderProgram> popShaderProgram();
        
        const Matrix4 *getProjectionMatrix() { return &projectionMatrix_; }
        
        void drawRect(Vector2 &position, Vector2 &size, Vector4 &color);
        void drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4]);
        void drawTexture(Vector2 &position, std::shared_ptr<Texture> texture);
        
        // Using the more updated means
        void drawFlatRect(GameObject *gameObject);
        void drawMaskRect(GameObject *gameObject);
        void drawLines(GameObject *gameObject);
        
        void drawSprite(GameObject *gameObject);

        uint8_t enableMask(GameObject *gameObject);
        void disableMask(uint8_t maskBits);
        
        void render();
        
        // TODO: This will get moved out
        void updateTransforms();
        
        void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
        
    private:
        Matrix4 currentMatrix_;
        std::vector<Matrix4> matrixStack_;
        ColorMatrix currentColorMatrix_;
        std::vector<ColorMatrix> colorMatrixStack_;
        ColorTransform currentColorTransform_;
        std::vector<ColorTransform> colorTransformStack_;
        bool texturingEnabled_[MaxTextureUnits];
        uint32_t currentTexture_[MaxTextureUnits];
        
        Matrix4 projectionMatrix_;
        GLKTextureInfo *textureInfo_;
        uint8_t activeMasks_;
        
        void queueRender(double deltaTime);
        
        int8_t renderGameObject(GameObject *gameObj, bool root, bool hasNextSibling = false);
        // TODO: This will get moved out
        void transformGameObject(GameObject *gameObj);

        void resetStacks();
        
        void pushMatrix();
        void popMatrix();
        
        void pushColorMatrix();
        void popColorMatrix();
        
        void pushColorTransform();
        void popColorTransform();
    };
}

#endif /* RenderServiceOpenGLES2_h */
