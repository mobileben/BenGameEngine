//
//  RenderServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#ifndef RenderServiceOpenGLES2_h
#define RenderServiceOpenGLES2_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include "RenderService.h"
#include "ShaderServiceOpenGLES2.h"
#include "MathTypes.h"
#include "Texture.h"
#include "Material.h"
#include "GameObject.h"
#include "Font.h"

#if TARGET_OS_IPHONE
#include <GLKit/GLKit.h>
#endif /* TARGET_OS_IPHONE */

namespace BGE {
    class TransformComponent;
    
    class RenderServiceOpenGLES2 : public RenderService
    {
    public:
        static const uint32_t MaxTextureUnits = 4;
        
        RenderServiceOpenGLES2();
        
        virtual void initialize() final;
        virtual void reset() final;
        virtual void platformSuspending() final;
        virtual void platformResuming() final;
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

        std::shared_ptr<ShaderProgram> useShaderProgram(const std::string& program);
        std::shared_ptr<ShaderProgram> pushShaderProgram(const std::string& program);
        std::shared_ptr<ShaderProgram> popShaderProgram();
        
        const Matrix4 *getProjectionMatrix() { return &projectionMatrix_; }
        
        void drawRect(Vector2 &position, Vector2 &size, Vector4 &color);
        void drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4]);
        void drawTexture(Vector2 &position, std::shared_ptr<Texture> texture);
        
        // Using the more updated means
        void drawFlatRect(GameObject *gameObject);
        void drawMaskRect(GameObject *gameObject);
        void drawTextureMask(GameObject *gameObject);
        void drawLines(GameObject *gameObject);
        void drawPolyLines(GameObject *gameObject);
        
        void drawSprite(GameObject *gameObject);

        void drawDebugQuads(std::vector<Vector3> points, Color &color);

        void drawString(TextComponent *text, Font *font, TransformComponent *transform, ColorMatrix& colorMatrix, ColorTransform& colorTransform, bool minimum=true);

        void drawString(std::vector<std::string> &strs, Font *font, float xOffset, float yOffset, std::vector<float> &yPos, float defWidth, TransformComponent *transform, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);

        uint8_t enableMask(GameObject *gameObject);
        void disableMask(uint8_t maskBits);
        
        void render();

#if TARGET_OS_IPHONE
        void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
#endif /* TARGET_OS_IPHONE */

#ifdef SUPPORT_PROFILING
        virtual void resetProfilingStats();
#endif /* SUPPORT_PROFILING */

    protected:
        virtual void createTexture(const RenderCommandItem& item);
        virtual void destroyTexture(const RenderCommandItem& item);

    private:
        ColorMatrix currentColorMatrix_;
        std::vector<ColorMatrix> colorMatrixStack_;
        ColorTransform currentColorTransform_;
        std::vector<ColorTransform> colorTransformStack_;
        bool texturingEnabled_[MaxTextureUnits];
        uint32_t currentTexture_[MaxTextureUnits];
        
        Matrix4 projectionMatrix_;
        GLKTextureInfo *textureInfo_;
        uint8_t activeMasks_;

        // TODO: Proper way for maintaining texture state would be for each texture unit as well as tracking the target
        GLuint currentTextureId_;
        std::shared_ptr<ShaderProgram> currentShader_;

        void queueRender(double deltaTime);
        
        int8_t renderGameObject(GameObject *gameObj, bool root, bool hasNextSibling = false);

        void resetStacks();
        
        void pushColorMatrix();
        void popColorMatrix();
        
        void pushColorTransform();
        void popColorTransform();

        void setTexture(GLenum target, GLuint texId);

        void drawString(std::string str, Font *font, const float *rawMatrix, float defWidth, float xOffset, float yOffset, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
    };
}

#endif /* RenderServiceOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
