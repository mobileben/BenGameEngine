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
#include "Texture.h"
#include "Profiling.h"
#include "Queue.h"

#include "vaser.h"

#include <thread>

#ifndef APPSTORE_RELEASE
#define DEBUG_RENDER_COMMAND
#endif /* APPSTORE_RELEASE */

namespace BGE {
    class ComponentService;
    class PolyLineRenderComponent;
    class Space;
    class TextComponent;
    class Texture;
    
    typedef Vector3 Vertex;
    
    typedef struct {
        Vector3 position;
        Vector4 color;
    } VertexColor;
    
    // VertexTex moved to Texture.h
    
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
    
    using CachedComponentRenderDataKey = uint64_t;
    
    struct CachedStringRenderData {
#if DEBUG
        uint32_t                lifetime;
#endif
#ifdef SUPPORT_OPENGL
        GLuint                  vbo;
        GLuint                  ibo;
        size_t                  numVertices;
        size_t                  numIndices;
        size_t                  maxIndices;
        size_t                  maxVertices;
        std::vector<GLushort>   indices;
        std::vector<VertexTex>  vertices;
#endif /* SUPPORT_OPENGL */
    };

    struct CachedPolyLineRenderData {
#if DEBUG
        uint32_t                                lifetime;
#endif
#ifdef SUPPORT_OPENGL
        GLuint                                  vbo;
        size_t                                  numVertices;
        size_t                                  maxVertices;
        VASEr::LineContext                      context;
        VASEr::LineRenderContext                renderContext;
#endif /* SUPPORT_OPENGL */
    };
    
    enum class RenderCommand { None, BindWindow, SetIsReady, CreateBuiltinShaders, CreateShader, Render, TextureCreate, TextureDestroy, VboCreate, VboDestroy, IboCreate, IboDestroy, CreateStringCacheEntry, DestroyStringCacheEntry, CreatePolyLineCacheEntry, DestroyPolyLineCacheEntry };

    struct RenderCommandData {
        virtual ~RenderCommandData() {}
    };

    struct RenderCommandBindWindowData : public RenderCommandData {
        std::shared_ptr<RenderContext>  context;
        std::shared_ptr<RenderWindow>   window;

        RenderCommandBindWindowData() = delete;
        RenderCommandBindWindowData(const std::shared_ptr<RenderContext>& context, const std::shared_ptr<RenderWindow>& window) : context(context), window(window) {}
    };

    struct RenderTextureCommandData : public RenderCommandData {
#ifdef DEBUG_RENDER_COMMAND
        std::string                 name;
#endif /* DEBUG_RENDER_COMMAND */
        TextureHandle               textureHandle;
        TextureFormat               textureFormat;
        uint8_t                     *textureBuffer;
        uint32_t                    textureWidth;
        uint32_t                    textureHeight;
#ifdef SUPPORT_OPENGL
        GLint                       glFormat;
        GLuint                      glHwId;
#endif /* SUPPORT_OPENGL */

        RenderTextureCommandData() : textureFormat(TextureFormat::Undefined), textureBuffer(nullptr), textureWidth(0), textureHeight(0) {
#ifdef SUPPORT_OPENGL
            glFormat = 0;
            glHwId = 0;
#endif /* SUPPORT_OPENGL */
        }
        
        RenderTextureCommandData(TextureHandle textureHandle) : textureHandle(textureHandle), textureFormat(TextureFormat::Undefined), textureBuffer(nullptr), textureWidth(0), textureHeight(0) {
#ifdef SUPPORT_OPENGL
            glFormat = 0;
            glHwId = 0;
#endif /* SUPPORT_OPENGL */
        }
        
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData(const std::string& name, TextureHandle textureHandle) : name(name), textureHandle(textureHandle), textureFormat(TextureFormat::Undefined), textureBuffer(nullptr), textureWidth(0), textureHeight(0) {
#ifdef SUPPORT_OPENGL
            glFormat = 0;
            glHwId = 0;
#endif /* SUPPORT_OPENGL */
        }
#endif /* DEBUG_RENDER_COMMAND */

#ifdef SUPPORT_OPENGL
        RenderTextureCommandData(TextureHandle textureHandle, GLuint hwId) : textureHandle(textureHandle), textureFormat(TextureFormat::Undefined), textureBuffer(nullptr), textureWidth(0), textureHeight(0) {
            glFormat = 0;
            glHwId = hwId;
        }
        
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData(const std::string& name, TextureHandle textureHandle, GLuint hwId) : name(name), textureHandle(textureHandle), textureFormat(TextureFormat::Undefined), textureBuffer(nullptr), textureWidth(0), textureHeight(0) {
            glFormat = 0;
            glHwId = hwId;
        }
#endif /* DEBUG_RENDER_COMMAND */
        
#endif /* SUPPORT_OPENGL */
        
        RenderTextureCommandData(TextureHandle textureHandle, TextureFormat format, uint8_t *buffer, uint32_t width, uint32_t height) : textureHandle(textureHandle), textureFormat(format), textureBuffer(buffer), textureWidth(width), textureHeight(height) {
#ifdef SUPPORT_OPENGL
            glFormat = 0;
            glHwId = 0;
#endif /* SUPPORT_OPENGL */
        }
        
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData(const std::string& name, TextureHandle textureHandle, TextureFormat format, uint8_t *buffer, uint32_t width, uint32_t height) : name(name), textureHandle(textureHandle), textureFormat(format), textureBuffer(buffer), textureWidth(width), textureHeight(height) {
#ifdef SUPPORT_OPENGL
            glFormat = 0;
            glHwId = 0;
#endif /* SUPPORT_OPENGL */
        }
#endif /* DEBUG_RENDER_COMMAND */
        
#ifdef SUPPORT_OPENGL
        RenderTextureCommandData(TextureHandle textureHandle, TextureFormat format, uint8_t *buffer, uint32_t width, uint32_t height, GLint glFormat) : textureHandle(textureHandle), textureFormat(format), textureBuffer(buffer), textureWidth(width), textureHeight(height) {
            this->glFormat = glFormat;
            glHwId = 0;
        }
        
#ifdef DEBUG_RENDER_COMMAND
        RenderTextureCommandData(const std::string& name, TextureHandle textureHandle, TextureFormat format, uint8_t *buffer, uint32_t width, uint32_t height, GLint glFormat) : name(name), textureHandle(textureHandle), textureFormat(format), textureBuffer(buffer), textureWidth(width), textureHeight(height) {
            this->glFormat = glFormat;
            glHwId = 0;
        }
#endif /* DEBUG_RENDER_COMMAND */
#endif /* SUPPORT_OPENGL */
    };

    struct RenderVboCommandData : public RenderCommandData {
        VertexTex   *vertexTexData;
        uint32_t    numVertexTex;
#ifdef SUPPORT_OPENGL
        GLuint  glVboId;
#endif /* SUPPORT_OPENGL */
        
        RenderVboCommandData() =delete;
        RenderVboCommandData(VertexTex *data, uint32_t num) : vertexTexData(data), numVertexTex(num) {
#ifdef SUPPORT_OPENGL
            glVboId = 0;
#endif /* SUPPORT_OPENGL */
        }
#ifdef SUPPORT_OPENGL
        RenderVboCommandData(GLuint id) : vertexTexData(nullptr), numVertexTex(0), glVboId(id) {}
#endif /* SUPPORT_OPENGL */
    };
    
    struct RenderIboCommandData : public RenderCommandData {
        void        *indices;
        uint32_t    numIndices;
        uint32_t    indexSize;
#ifdef SUPPORT_OPENGL
        GLuint      glIboId;
#endif /* SUPPORT_OPENGL */
        
        RenderIboCommandData() =delete;
        RenderIboCommandData(void *indices, uint32_t num, uint32_t size) : indices(indices), numIndices(num), indexSize(size) {
#ifdef SUPPORT_OPENGL
            glIboId = 0;
#endif /* SUPPORT_OPENGL */
        }
#ifdef SUPPORT_OPENGL
        RenderIboCommandData(GLuint id) : indices(nullptr), numIndices(0), indexSize(0), glIboId(id) {}
#endif /* SUPPORT_OPENGL */
    };
    
    struct RenderStringCacheCommandData : public RenderCommandData {
        SpaceHandle         spaceHandle;
        TextComponentHandle textHandle;
        
        RenderStringCacheCommandData() = delete;
        RenderStringCacheCommandData(SpaceHandle spaceHandle, TextComponentHandle textHandle) : spaceHandle(spaceHandle), textHandle(textHandle) {}
    };
    
    struct RenderPolyLineCacheCommandData : public RenderCommandData {
        SpaceHandle                             spaceHandle;
        PolyLineRenderComponentHandle           polyHandle;
        
        RenderPolyLineCacheCommandData() = delete;
        RenderPolyLineCacheCommandData(SpaceHandle spaceHandle, PolyLineRenderComponentHandle polyHandle) : spaceHandle(spaceHandle), polyHandle(polyHandle) {}
    };

    struct RenderCommandItem {
        RenderCommand                                                   command;
        std::shared_ptr<RenderCommandData>                              data;
        std::function<void(RenderCommandItem, std::shared_ptr<Error>)>  callback;

        RenderCommandItem() : command(RenderCommand::None) {}
        RenderCommandItem(RenderCommand command) : command(command) {}
        RenderCommandItem(RenderCommand command, const std::shared_ptr<RenderCommandData>& data) : command(command), data(data) {}
        RenderCommandItem(RenderCommand command, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) : command(command), callback(callback) {}
        RenderCommandItem(RenderCommand command, const std::shared_ptr<RenderCommandData>& data, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback) : command(command), data(data), callback(callback) {}
    };

    enum class RenderServiceError : int32_t { None = 0, Unsupported, Unimplemented, IllegalRenderCommand, RenderCommandMissingData, RenderCommandMalformed };

    class RenderService: public Service
    {
    public:
        static const std::string ErrorDomain;

        RenderService();
        virtual ~RenderService();

        virtual void initialize() final;

        virtual void bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window);
        virtual void resizeRenderWindow();
        virtual void createShaders();
        
        virtual size_t totalMemory() const { return 0; }
        
        bool isReady() const { return ready_; }
        void setIsReady() { ready_ = true; }
        
        Render2DCoordinateSystem getCoordinateSystem2D() const { return coordSystem2D_; }
        
        // Y-up is inverted, Y-down is normal
        bool hasInvertedYAxis() const { return invertedYAxis_; }
        
        virtual void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        std::shared_ptr<RenderContext> getRenderContext() const { return renderContext_; }
        
        std::shared_ptr<RenderWindow> getRenderWindow() const { return renderWindow_; }
        
        std::shared_ptr<ShaderService> getShaderService() const { return shaderService_; }
        
        std::vector<Vector3> &getBoundBoxPoints() { return boundingBoxPoints_; }
        std::vector<Vector3> &getScaledBoundBoxPoints() { return scaledBoundingBoxPoints_; }
        
        Color getBackgroundColor() const { return backgroundColor_; }
        void setBackgroundColor(Color color) { backgroundColor_ = color; }
        
        Vector2 getWindowWidthHeight() const;
        Vector2 getWindowMappedWidthHeight() const;

        Vector2 deviceCoordinatesFromRenderCoordinates(Vector2 pos);
        
        void setComponentService(std::shared_ptr<ComponentService> componentService);

#ifdef SUPPORT_PROFILING
        double getFps(void) const { return frameRateCalculator_.getFps(); }
        int64_t getCurrentFrameTime(void) { return frameRateCalculator_.getCurrentFrameTime(); }

        int32_t getNumGameObjectsDrawn() const { return numGameObjectsDrawn_; }
        int32_t getNumGameObjectsIgnored() const { return numGameObjectsIgnored_; }
        int32_t getNumDrawCalls() const { return numDrawCalls_; }
        int32_t getNumSpritesDrawn() const { return numSpritesDrawn_; }
        int32_t getNumFontCharactersDrawn() const { return numFontCharactersDrawn_; }
        int32_t getNumPolylinesDrawn() const { return numPolylinesDrawn_; }
        int32_t getNumLinesDrawn() const { return numLinesDrawn_; }
        int32_t getNumRectsDrawn() const { return numRectsDrawn_; }
        int32_t getNumMasksDrawn() const { return numMasksDrawn_; }
        int32_t getNumShadersChanged() const { return numShadersChanged_; }
        int32_t getNumTexturesChanged() const { return numTexturesChanged_; }
        int32_t getNumProcessedObjects() const { return numProcessedObjects_; }
        int64_t getProcessingTime() const { return processingTime_; }
#endif /* SUPPORT_PROFILING */

        virtual std::shared_ptr<ShaderProgram> useShaderProgram(ShaderProgramId progam, bool& changed) =0;
        virtual std::shared_ptr<ShaderProgram> useShaderProgram(const std::string& program, bool& changed) =0;
        virtual std::shared_ptr<ShaderProgram> pushShaderProgram(const std::string& program) =0;
        virtual std::shared_ptr<ShaderProgram> popShaderProgram() =0;
        
        virtual void render() =0;

        virtual void windowMappedDimensionsUpdated(__attribute__((unused)) std::shared_ptr<RenderWindow> window);
        
        // Queuing methods
        void queueBindRenderWindow(const std::shared_ptr<RenderContext>& context, const std::shared_ptr<RenderWindow>& window);
        void queueCreateBuiltinShaders();
        void queueSetIsReady();
        void queueCreateTexture(const RenderTextureCommandData& texData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueDestroyTexture(const RenderTextureCommandData& texData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueCreateVbo(const RenderVboCommandData& vboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueDestroyVbo(const RenderVboCommandData& vboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueCreateIbo(const RenderIboCommandData& iboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueDestroyIbo(const RenderIboCommandData& iboData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueCreateStringCacheEntry(const RenderStringCacheCommandData& cacheData);
        void queueDestroyStringCacheEntry(const RenderStringCacheCommandData& cacheData);
        void queueCreatePolyLineCacheEntry(const RenderPolyLineCacheCommandData& cacheData, std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);
        void queueDestroyPolyLineCacheEntry(const RenderPolyLineCacheCommandData& cacheData);
        void queueRender(std::function<void(RenderCommandItem, std::shared_ptr<Error>)> callback);

#if DEBUG
        virtual uint32_t numVbo() const { return 0; }
        virtual uint32_t numIbo() const { return 0; }
#endif /* DEBUG */

    protected:
        std::shared_ptr<RenderContext> renderContext_;
        std::shared_ptr<RenderWindow> renderWindow_;
        std::shared_ptr<ShaderService> shaderService_;
        
        std::vector<std::shared_ptr<ShaderProgram>> shaderProgramStack_;
        
        std::shared_ptr<ComponentService>   componentService_;
        
        std::vector<Vector3> boundingBoxPoints_;
        std::vector<Vector3> scaledBoundingBoxPoints_;

        Queue<RenderCommandItem>    renderQueue_;

        virtual void threadCleanup() {}
        
        virtual void createTexture(const RenderCommandItem& item);
        virtual void destroyTexture(const RenderCommandItem& item);
        virtual void createVbo(const RenderCommandItem& item);
        virtual void destroyVbo(const RenderCommandItem& item);
        virtual void createIbo(const RenderCommandItem& item);
        virtual void destroyIbo(const RenderCommandItem& item);
        virtual void createStringCacheEntry(const RenderCommandItem& item);
        virtual void destroyStringCacheEntry(const RenderCommandItem& item);
        virtual void createPolyLineCacheEntry(const RenderCommandItem& item);
        virtual void destroyPolyLineCacheEntry(const RenderCommandItem& item);

        template <typename T>
        CachedComponentRenderDataKey getCachedComponentRenderDataKey(SpaceHandle spaceHandle, T componentHandle) {
            return static_cast<uint64_t>(spaceHandle.getHandle()) << 32 | static_cast<uint64_t>(componentHandle.getHandle());
        }
        
        SpaceHandle getSpaceHandleFromCachedComponentRenderDataKey(CachedComponentRenderDataKey key);
        
        template <typename T>
        Handle<T> getComponentHandleFromCachedComponentRenderDataKey(CachedComponentRenderDataKey key) {
            return Handle<T>(static_cast<HandleBackingType>(key&0xFFFFFFFF));
        }

#ifdef SUPPORT_PROFILING
        profiling::FrameRateCalculator frameRateCalculator_;

        int32_t numGameObjectsDrawn_;
        int32_t numGameObjectsIgnored_;
        int32_t numDrawCalls_;
        int32_t numSpritesDrawn_;
        int32_t numFontCharactersDrawn_;
        int32_t numPolylinesDrawn_;
        int32_t numLinesDrawn_;
        int32_t numRectsDrawn_;
        int32_t numMasksDrawn_;
        int32_t numShadersChanged_;
        int32_t numTexturesChanged_;
        int32_t numBlendChanged_;
        int32_t numBlendFuncChanged_;
        int32_t numProcessedObjects_;
        int64_t processingTime_;

        virtual void resetProfilingStats();
#endif /* SUPPORT_PROFILING */

    private:
        bool ready_;
        bool invertedYAxis_;    // Y-up is inverted, Y-down is normal
        Render2DCoordinateSystem coordSystem2D_;
        
        Color backgroundColor_;

        
        bool            threadRunning_;
        std::mutex      threadRunningMutex_;
        std::thread     thread_;

        virtual void threadFunction();
    };
}

#endif /* BGERenderService_h */
