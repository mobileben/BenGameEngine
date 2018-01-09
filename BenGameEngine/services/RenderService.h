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
#include "Profiling.h"

namespace BGE {
    class ComponentService;
    
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
        
        virtual size_t totalMemory() const { return 0; }
        
        bool isReady() const { return ready_; }
        void setIsReady() { ready_ = true; }
        
        Render2DCoordinateSystem getCoordinateSystem2D() const { return coordSystem2D_; }
        
        // Y-up is inverted, Y-down is normal
        bool hasInvertedYAxis() const { return invertedYAxis_; }
        
        void setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D);
        
        std::shared_ptr<RenderContext> getRenderContext() const { return renderContext_; }
        
        std::shared_ptr<RenderWindow> getRenderWindow() const { return renderWindow_; }
        
        std::shared_ptr<ShaderService> getShaderService() const { return shaderService_; }
        
        std::vector<Vector3> &getBoundBoxPoints() { return boundingBoxPoints_; }
        std::vector<Vector3> &getScaledBoundBoxPoints() { return scaledBoundingBoxPoints_; }
        
        Color getBackgroundColor() const { return backgroundColor_; }
        void setBackgroundColor(Color color) { backgroundColor_ = color; }
        
        Vector2 getWindowWidthHeight() const;

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

        virtual std::shared_ptr<ShaderProgram> useShaderProgram(const std::string& program) =0;
        virtual std::shared_ptr<ShaderProgram> pushShaderProgram(const std::string& program) =0;
        virtual std::shared_ptr<ShaderProgram> popShaderProgram() =0;
        
        virtual void render() =0;
        
    protected:
        std::shared_ptr<RenderContext> renderContext_;
        std::shared_ptr<RenderWindow> renderWindow_;
        std::shared_ptr<ShaderService> shaderService_;
        
        std::vector<std::shared_ptr<ShaderProgram>> shaderProgramStack_;
        
        std::shared_ptr<ComponentService>   componentService_;
        
        std::vector<Vector3> boundingBoxPoints_;
        std::vector<Vector3> scaledBoundingBoxPoints_;

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
        int32_t numProcessedObjects_;
        int64_t processingTime_;

        virtual void resetProfilingStats();
#endif /* SUPPORT_PROFILING */

        
    private:
        bool ready_;
        bool invertedYAxis_;    // Y-up is inverted, Y-down is normal
        Render2DCoordinateSystem coordSystem2D_;
        
        Color backgroundColor_;
    };
}

#endif /* BGERenderService_h */
