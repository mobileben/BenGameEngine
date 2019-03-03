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
        ~RenderServiceOpenGLES2() {}
        
        virtual void reset() final;
        virtual void platformSuspending() final;
        virtual void platformResuming() final;
        virtual void pause() final;
        virtual void resume() final;
        virtual void destroy() final;
        void update(__attribute__ ((unused)) double deltaTime) final {}

        static int8_t MaxActiveMasks;
        
        size_t totalMemory() const final;

        void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        void bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window);
        void resizeRenderWindow();
        void createShaders();

        std::shared_ptr<ShaderProgram> useShaderProgram(ShaderProgramId program, bool& changed);
        std::shared_ptr<ShaderProgram> useShaderProgram(const std::string& program, bool& changed);
        std::shared_ptr<ShaderProgram> pushShaderProgram(const std::string& program);
        std::shared_ptr<ShaderProgram> popShaderProgram();
        
        const Matrix4 *getProjectionMatrix() { return &projectionMatrix_; }
        const Matrix4 *getMappedProjectionMatrix() { return &mappedProjectionMatrix_; }
        
        void drawRect(Vector2 &position, Vector2 &size, Vector4 &color);
        void drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4]);
        void drawTexture(Vector2 &position, std::shared_ptr<Texture> texture);
        
        // Using the more updated means
        void drawFlatRect(Space *space, GameObject *gameObject, TransformComponent *transform);
        void drawMaskRect(Space *space, GameObject *gameObject, TransformComponent *transform);
        void drawTextureMask(Space *space, GameObject *gameObject, TransformComponent *transform);
        void drawLines(Space *space, GameObject *gameObject, TransformComponent *transform);
        void drawPolyLines(Space *space, GameObject *gameObject, TransformComponent *transform);
        
        void drawSprite(Space *space, GameObject *gameObject, TransformComponent *transform);

        void drawDebugQuads(std::vector<Vector3> points, Color &color);

        void drawString(Space *space, TextComponent *text, Font *font, TransformComponent *transform, ColorMatrix& colorMatrix, ColorTransform& colorTransform, bool minimum=true);
        
        uint8_t enableMask(Space *space, GameObject *gameObject, TransformComponent *transform);
        void disableMask(uint8_t maskBits);
        
        void render();
        
        virtual void windowMappedDimensionsUpdated(std::shared_ptr<RenderWindow> window);
        
#if TARGET_OS_IPHONE
        void setGLKTextureInfo(GLKTextureInfo *info) { textureInfo_ = info; }
#endif /* TARGET_OS_IPHONE */

#ifdef SUPPORT_PROFILING
        virtual void resetProfilingStats();
#endif /* SUPPORT_PROFILING */

    protected:
        enum class BlendFunc : uint32_t { Src_ONE_Dst_ZERO, Src_ONE_Dst_ONE_MINUS_SRC_ALPHA, Src_SRC_ALPHA_Dst_ONE_MINUS_SRC_ALPHA };
        
        virtual void threadCleanup();

        virtual void createTexture(const RenderCommandItem& item);
        virtual void destroyTexture(const RenderCommandItem& item);

        virtual void createVbo(const RenderCommandItem& item);
        virtual void destroyVbo(const RenderCommandItem& item);
        
        GLuint createVbo_(VertexTex *data, uint32_t numVertexTex);
        void destroyVbo_(GLuint vbo);

        virtual void createIbo(const RenderCommandItem& item);
        virtual void destroyIbo(const RenderCommandItem& item);
        
        GLuint createIbo_(void *indices, uint32_t numIndices, uint32_t indexSize);
        void destroyIbo_(GLuint ibo);

        virtual void createStringCacheEntry(const RenderCommandItem& item);
        virtual void destroyStringCacheEntry(const RenderCommandItem& item);

    private:
        ColorMatrix currentColorMatrix_;
        std::vector<ColorMatrix> colorMatrixStack_;
        ColorTransform currentColorTransform_;
        std::vector<ColorTransform> colorTransformStack_;
        bool texturingEnabled_[MaxTextureUnits];
        uint32_t currentTexture_[MaxTextureUnits];
        
        Matrix4 projectionMatrix_;
        Matrix4 mappedProjectionMatrix_;
        Matrix4 identifyMatrix_;
        
        GLKTextureInfo *textureInfo_;
        uint8_t activeMasks_;

        // TODO: Proper way for maintaining texture state would be for each texture unit as well as tracking the target
        GLuint                          currentTextureId_;
        ShaderProgramId                 currentShaderProgramId_;
        std::shared_ptr<ShaderProgram>  currentShaderProgram_;

        // Render states
        bool                            currentBlend_;
        BlendFunc                       currentBlendFunc_;
        GLfloat                         currentLineWidth_;
        GLuint                          currentVbo_;
        GLuint                          currentIbo_;
        
        std::vector<bool>               currentVertexAttributes_;

        std::vector<SpaceHandle>                            spaceHandles_;
        std::vector<GameObject *>                           rootGameObjects_;
        std::vector<std::vector<TransformComponentHandle>>  orderedChildrenHandles_;
        
        std::vector<VertexTex>                              stringVertexData_;
        std::vector<GLushort>                               stringIndices_;
        
        // String caches
        std::map<CachedStringRenderDataKey, CachedStringRenderData> stringVertexCache_;
        std::map<CachedStringRenderDataKey, CachedStringRenderData> dropShadowStringVertexCache_;

        void queueRender(double deltaTime);
        
        int8_t renderGameObject(GameObject *gameObj, Space *space, uint32_t depth, bool hasNextSibling = false);
        
        void resetStacks();
        
        void pushColorMatrix();
        void popColorMatrix();
        
        void pushColorTransform();
        void popColorTransform();

        bool setTexture(GLenum target, GLuint texId);
        bool setBlend(bool blend);
        bool setBlendFunc(BlendFunc blendFunc);
        bool setLineWidth(GLfloat width);
        bool setVbo(GLuint vbo);
        bool setIbo(GLuint ibo);
        void disableVboIbo();
        
        void setVertexAttribute(GLuint index, bool enabled);
        
        void drawString(CachedStringRenderData *cache, std::vector<std::string> &strs, bool dirty, Font *font, float xOffset, float yOffset, std::vector<float> &yPos, float defWidth, TransformComponent *transform, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
        void drawString(CachedStringRenderData *cache, const std::string& str, bool dirty, bool last, Font *font, const float *rawMatrix, float defWidth, float xOffset, float yOffset, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
    };
}

#endif /* RenderServiceOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
