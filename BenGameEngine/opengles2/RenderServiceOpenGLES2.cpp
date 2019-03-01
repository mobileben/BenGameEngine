//
//  RenderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#include "RenderServiceOpenGLES2.h"
#include "RenderView.h"
#include "ShaderServiceOpenGLES2.h"
#include "RenderContextOpenGLES2.h"
#include "MathTypes.h"
#include "Game.h"
#include "LineRenderComponent.h"
#include "PolyLineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "TextureMaskComponent.h"
#include "MaskComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "vaser.h"
#include "backend.h"

#ifdef SUPPORT_PROFILING
#include "Profiling.h"
#endif /* SUPPORT_PROFILING */

#define USE_IBO

const GLubyte VertexIndices[] = {
    0, 3, 2,
    0, 2, 1
};

int8_t BGE::RenderServiceOpenGLES2::MaxActiveMasks = 8;
static uint8_t MaskIdToMaskValue[] = {
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128
};

static size_t kDefaultOrderedChildrenHandlesSize = 32;

// Shaders and Programs

namespace BGE {
    static const ShaderId SimpleVertexShaderId = 0;
    static const ShaderId SimpleFragmentShaderId = 1;
    static const ShaderId LineVertexShaderId = 2;
    static const ShaderId LineFragmentShaderId = 3;
    static const ShaderId TextureVertexShaderId = 4;
    static const ShaderId TextureFragmentShaderId = 5;
    static const ShaderId ColorMatrixTextureVertexShaderId = 6;
    static const ShaderId ColorMatrixTextureFragmentShaderId = 7;
    static const ShaderId FullColorTextureVertexShaderId = 8;
    static const ShaderId FullColorTextureFragmentShaderId = 9;
    static const ShaderId MaskColorMatrixTextureVertexShaderId = 10;
    static const ShaderId MaskColorMatrixTextureFragmentShaderId = 11;
    static const ShaderId FontVertexShaderId = 12;
    static const ShaderId FontFragmentShaderId = 13;
    static const ShaderId FullColorFontVertexShaderId = 14;
    static const ShaderId FullColorFontFragmentShaderId = 15;
    static const ShaderId PolyLineVertexShaderId = 16;
    static const ShaderId PolyLineFragmentShaderId = 17;
    
    static const ShaderProgramId SimpleShaderProgramId = 0;
    static const ShaderProgramId LineShaderProgramId = 1;
    static const ShaderProgramId TextureShaderProgramId = 2;
    static const ShaderProgramId ColorMatrixTextureShaderProgramId = 3;
    static const ShaderProgramId FullColorTextureShaderProgramId = 4;
    static const ShaderProgramId MaskColorMatrixTextureShaderProgramId = 5;
    static const ShaderProgramId FontShaderProgramId = 6;
    static const ShaderProgramId FullColorFontShaderProgramId = 7;
    static const ShaderProgramId PolyLineShaderProgramId = 8;

    static const std::string SimpleVertexShaderName = "SimpleVertex";
    static const std::string SimpleFragmentShaderName = "SimpleFragment";
    static const std::string SimpleShaderProgramName = "Default";
    
    static const std::string LineVertexShaderName = "LineVertex";
    static const std::string LineFragmentShaderName = "LineFragment";
    static const std::string LineShaderProgramName = "Line";

    static const std::string TextureVertexShaderName = "TextureVertex";
    static const std::string TextureFragmentShaderName = "TextureFragment";
    static const std::string TextureShaderProgramName = "Texture";

    static const std::string ColorMatrixTextureVertexShaderName = "ColorMatrixTextureVertex";
    static const std::string ColorMatrixTextureFragmentShaderName = "ColorMatrixTextureFragment";
    static const std::string ColorMatrixTextureShaderProgramName = "ColorMatrixTexture";

    static const std::string FullColorTextureVertexShaderName = "FullColorTextureVertex";
    static const std::string FullColorTextureFragmentShaderName = "FullColorTextureFragment";
    static const std::string FullColorTextureShaderProgramName = "FullColorTexture";

    static const std::string MaskColorMatrixTextureVertexShaderName = "MaskColorMatrixTextureVertex";
    static const std::string MaskColorMatrixTextureFragmentShaderName = "MaskColorMatrixTextureFragment";
    static const std::string MaskColorMatrixTextureShaderProgramName = "MaskColorMatrixTexture";

    static const std::string FontVertexShaderName = "FontVertex";
    static const std::string FontFragmentShaderName = "FontFragment";
    static const std::string FontShaderProgramName = "Font";

    static const std::string FullColorFontVertexShaderName = "FullColorFontVertex";
    static const std::string FullColorFontFragmentShaderName = "FullColorFontFragment";
    static const std::string FullColorFontShaderProgramName = "FullColorFont";

    static const std::string PolyLineVertexShaderName = "PolyLineVertex";
    static const std::string PolyLineFragmentShaderName = "PolyLineFragment";
    static const std::string PolyLineShaderProgramName = "PolyLine";

    static const ShaderAttributeId MaskTexCoordInShaderAttributeId = 0;
    static const ShaderAttributeId PositionShaderAttributeId = 1;
    static const ShaderAttributeId SourceColorShaderAttributeId = 2;
    static const ShaderAttributeId TexCoordInShaderAttributeId = 3;

    static const std::string MaskTexCoordInShaderAttributeName = "MaskTexCoordIn";
    static const std::string PositionShaderAttributeName = "Position";
    static const std::string SourceColorShaderAttributeName = "SourceColor";
    static const std::string TexCoordInShaderAttributeName = "TexCoordIn";
    
    static const ShaderUniformId ColorShaderUniformId = 0;
    static const ShaderUniformId ColorMatOffsetShaderUniformId = 1;
    static const ShaderUniformId ColorMatrixShaderUniformId = 2;
    static const ShaderUniformId ColorMultiplierShaderUniformId = 3;
    static const ShaderUniformId ColorOffsetShaderUniformId = 4;
    static const ShaderUniformId MaskTextureShaderUniformId = 5;
    static const ShaderUniformId ModelViewShaderUniformId = 6;
    static const ShaderUniformId ProjectionShaderUniformId = 7;
    static const ShaderUniformId SourceColorShaderUniformId = 8;
    static const ShaderUniformId TextureShaderUniformId = 9;
    
    static const std::string ColorShaderUniformName = "Color";
    static const std::string ColorMatOffsetShaderUniformName = "ColorMatOffset";
    static const std::string ColorMatrixShaderUniformName = "ColorMatrix";
    static const std::string ColorMultiplierShaderUniformName = "ColorMultiplier";
    static const std::string ColorOffsetShaderUniformName = "ColorOffset";
    static const std::string MaskTextureShaderUniformName = "MaskTexture";
    static const std::string ModelViewShaderUniformName = "ModelView";
    static const std::string ProjectionShaderUniformName = "Projection";
    static const std::string SourceColorShaderUniformName = "SourceColor";
    static const std::string TextureShaderUniformName = "Texture";
}

#ifdef DEBUG_OPEN_GL
void CheckGLError() {
    auto err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("GL ERROR %d\n", err);
    }
}
#endif

BGE::RenderServiceOpenGLES2::RenderServiceOpenGLES2() : activeMasks_(0), indexBufferId_(0), currentTextureId_(0), currentShaderProgramId_(ShaderProgramIdUndefined) {
    shaderService_ = std::make_shared<ShaderServiceOpenGLES2>();
    ShaderServiceOpenGLES2::mapShaderBundle("BenGameEngineBundle");
    
    Matrix4MakeIdentity(projectionMatrix_);
    Matrix4MakeIdentity(mappedProjectionMatrix_);
    Matrix4MakeIdentity(identifyMatrix_);

    currentBlend_ = false;
    currentBlendFunc_ = BlendFunc::Src_ONE_Dst_ZERO;
    currentLineWidth_ = 1;
    currentVbo_ = 0;
    currentIbo_ = 0;
    
#ifdef SUPPORT_PROFILING
    resetProfilingStats();
#endif /* SUPPORT_PROFILING */
    
    for (uint32_t i=0;i<MaxTextureUnits;++i) {
        texturingEnabled_[i] = false;
        currentTexture_[i] = 0;
    }
    
    orderedChildrenHandles_ = std::vector<std::vector<TransformComponentHandle>>(kDefaultOrderedChildrenHandlesSize, std::vector<TransformComponentHandle>());
    for (auto& handles : orderedChildrenHandles_) {
        handles.reserve(kDefaultOrderedChildrenHandlesSize);
    }
}

void BGE::RenderServiceOpenGLES2::reset() {}

void BGE::RenderServiceOpenGLES2::platformSuspending() {
    lock();
    
    Service::platformSuspending();
    
    unlock();
}

void BGE::RenderServiceOpenGLES2::platformResuming() {
    lock();
    
    Service::platformResuming();
    
    unlock();
}

void BGE::RenderServiceOpenGLES2::pause() {
    lock();
    
    Service::pause();
    
    unlock();
}

void BGE::RenderServiceOpenGLES2::resume() {
    lock();
    
    Service::resume();
    
    unlock();
}

void BGE::RenderServiceOpenGLES2::destroy() {}

size_t BGE::RenderServiceOpenGLES2::totalMemory() const {
    size_t memory = 0;

#if TARGET_OS_IPHONE
    auto window = this->getRenderWindow();
    if (window) {
        auto view = this->getRenderWindow()->getView();
        if (view) {
            auto area = view.drawableWidth * view.drawableHeight;
            switch (view.drawableColorFormat) {
                case GLKViewDrawableColorFormatRGBA8888:
                case GLKViewDrawableColorFormatSRGBA8888:
                    memory += area * 4;
                    break;
                    
                case GLKViewDrawableColorFormatRGB565:
                    memory += area * 2;
                    break;
                    
                default:
                    break;
            }
            
            switch (view.drawableStencilFormat) {
                case GLKViewDrawableStencilFormat8:
                    memory += area;
                    break;
                    
                default:
                    break;
            }
            
            switch (view.drawableDepthFormat) {
                case GLKViewDrawableDepthFormat24:
                    memory += area * 3;
                    break;
                    
                case GLKViewDrawableDepthFormat16:
                    memory += area * 2;
                    break;
                    
                default:
                    break;
            }
        }
    }
#else
    // For now assume 32-bit color buffer only
#endif /* TARGET_OS_IPHONE */
    return memory;
    
}

void BGE::RenderServiceOpenGLES2::setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D)
{
    RenderService::setCoordinateSystem2D(coordSystem2D);
    
    std::shared_ptr<RenderWindow> window = this->getRenderWindow();
    
    switch (coordSystem2D) {
        case Render2DCoordinateSystem::Traditional:
            Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor(), window->getHeight() * this->getRenderWindow()->getContentScaleFactor(), 0, -1, 1);
            break;
        case Render2DCoordinateSystem::TraditionalCentered:
            Matrix4MakeOrthographic(projectionMatrix_, -window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -1, 1);
            break;
        case Render2DCoordinateSystem::OpenGL:
            Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor(), 0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor(), -1, 1);
            break;
        case Render2DCoordinateSystem::OpenGLCentered:
            Matrix4MakeOrthographic(projectionMatrix_, -window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -1, 1);
            break;
    }
    mappedProjectionMatrix_ = projectionMatrix_;
}

void BGE::RenderServiceOpenGLES2::bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window)
{
#if TARGET_OS_IPHONE
    auto glContext = std::dynamic_pointer_cast<RenderContextOpenGLES2>(context);

    [EAGLContext setCurrentContext:glContext->getContext()];

    RenderService::bindRenderWindow(context, window);

    if (glContext) {
        window->getView().context = glContext->getContext();
        window->getView().drawableStencilFormat = GLKViewDrawableStencilFormat8;
//        window->getView().drawableMultisample = GLKViewDrawableMultisample4X;
    }
    
    // Create the ortho matrix
//    Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), window->getHeight(), 0, -1, 1);
//    Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), 0, window->getHeight(), -1, 1);
//    Matrix4Scale(projectionMatrix_, 1, -1, 1);
//    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)window->getView().layer;
//    eaglLayer.contentsScale = window->getView().contentScaleFactor;
#endif /* TARGET_OS_IPHONE */

    setCoordinateSystem2D(Render2DCoordinateSystem::OpenGLCentered);
}

void BGE::RenderServiceOpenGLES2::resizeRenderWindow()
{
}

void BGE::RenderServiceOpenGLES2::createShaders()
{
    auto firstUseFunction = [](ShaderProgram *program) {
        auto shader = static_cast<ShaderProgramOpenGLES2 *>(program);
        GLint textureUniform = shader->locationForUniform(TextureShaderUniformId);
        glUniform1i(textureUniform, 0);
    };
    auto mappedFunction = [this](ShaderProgram *program) {
        auto shader = static_cast<ShaderProgramOpenGLES2 *>(program);
        GLint projectionLocation = shader->locationForUniform(ProjectionShaderUniformId);
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) getMappedProjectionMatrix()->m);
    };
    
    std::shared_ptr<Shader> vShader = this->getShaderService()->createShader(ShaderType::Vertex, SimpleVertexShaderId, SimpleVertexShaderName);
    std::shared_ptr<Shader> fShader = this->getShaderService()->createShader(ShaderType::Fragment, SimpleFragmentShaderId, SimpleFragmentShaderName);
    std::shared_ptr<ShaderProgram> program = this->getShaderService()->createShaderProgram(SimpleShaderProgramId, SimpleShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {SourceColorShaderAttributeId, SourceColorShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}}, nullptr, mappedFunction);

    vShader = this->getShaderService()->createShader(ShaderType::Vertex, LineVertexShaderId, LineVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, LineFragmentShaderId, LineFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(LineShaderProgramId, LineShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {ColorShaderUniformId, ColorShaderUniformName}}, nullptr, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, TextureVertexShaderId, TextureVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, TextureFragmentShaderId, TextureFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(TextureShaderProgramId, TextureShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {SourceColorShaderAttributeId, SourceColorShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, ColorMatrixTextureVertexShaderId, ColorMatrixTextureVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, ColorMatrixTextureFragmentShaderId, ColorMatrixTextureFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(ColorMatrixTextureShaderProgramId, ColorMatrixTextureShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}, {ColorMatrixShaderUniformId, ColorMatrixShaderUniformName}, {ColorMatOffsetShaderUniformId, ColorMatOffsetShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, FullColorTextureVertexShaderId, FullColorTextureVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, FullColorTextureFragmentShaderId, FullColorTextureFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(FullColorTextureShaderProgramId, FullColorTextureShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}, {ColorMatOffsetShaderUniformId, ColorMatOffsetShaderUniformName}, {ColorMatrixShaderUniformId, ColorMatrixShaderUniformName}, {ColorMultiplierShaderUniformId, ColorMultiplierShaderUniformName}, {ColorOffsetShaderUniformId, ColorOffsetShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, MaskColorMatrixTextureVertexShaderId, MaskColorMatrixTextureVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, MaskColorMatrixTextureFragmentShaderId, MaskColorMatrixTextureFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(MaskColorMatrixTextureShaderProgramId, MaskColorMatrixTextureShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}, {MaskTexCoordInShaderAttributeId, MaskTexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}, {MaskTextureShaderUniformId, MaskTextureShaderUniformName}, {ColorMatrixShaderUniformId, ColorMatrixShaderUniformName}, {ColorMatOffsetShaderUniformId, ColorMatOffsetShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, FontVertexShaderId, FontVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, FontFragmentShaderId, FontFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(FontShaderProgramId, FontShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {SourceColorShaderUniformId, SourceColorShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, FullColorFontVertexShaderId, FullColorFontVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, FullColorFontFragmentShaderId, FullColorFontFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(FullColorFontShaderProgramId, FullColorFontShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {TexCoordInShaderAttributeId, TexCoordInShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}, {TextureShaderUniformId, TextureShaderUniformName}, {SourceColorShaderUniformId, SourceColorShaderUniformName}, {ColorMatOffsetShaderUniformId, ColorMatOffsetShaderUniformName}, {ColorMatrixShaderUniformId, ColorMatrixShaderUniformName}, {ColorMultiplierShaderUniformId, ColorMultiplierShaderUniformName}, {ColorOffsetShaderUniformId, ColorOffsetShaderUniformName}}, firstUseFunction, mappedFunction);
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, PolyLineVertexShaderId, PolyLineVertexShaderName);
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, PolyLineFragmentShaderId, PolyLineFragmentShaderName);
    program = this->getShaderService()->createShaderProgram(PolyLineShaderProgramId, PolyLineShaderProgramName, {vShader,  fShader}, {{PositionShaderAttributeId, PositionShaderAttributeName}, {SourceColorShaderAttributeId, SourceColorShaderAttributeName}}, {{ModelViewShaderUniformId, ModelViewShaderUniformName}, {ProjectionShaderUniformId, ProjectionShaderUniformName}}, nullptr, mappedFunction);
}

std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::useShaderProgram(ShaderProgramId program, bool& changed) {
    changed = false;
    if (currentShaderProgramId_ != program) {
        auto shaderProgram = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderService_->getShaderProgram(program));
        if (shaderProgram) {
            glUseProgram(shaderProgram->getProgram());
            currentShaderProgramId_ = program;
            currentShaderProgram_ = shaderProgram;
            changed = true;
#ifdef SUPPORT_PROFILING
            ++numShadersChanged_;
#endif /* SUPPORT_PROFILING */
        }
    }
    return currentShaderProgram_;
}

std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::useShaderProgram(const std::string& program, bool& changed) {
    changed = false;
    
    auto shaderProgram = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderService_->getShaderProgram(program));
    if (shaderProgram) {
        if (currentShaderProgram_ != shaderProgram) {
            glUseProgram(shaderProgram->getProgram());
            currentShaderProgramId_ = shaderProgram->getId();
            currentShaderProgram_ = shaderProgram;
            changed = true;
#ifdef SUPPORT_PROFILING
            ++numShadersChanged_;
#endif /* SUPPORT_PROFILING */
        }
    }
    return currentShaderProgram_;
}

std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::pushShaderProgram(const std::string& program)
{
    std::string currShader;
    
    if (!shaderProgramStack_.empty()) {
        currShader = shaderProgramStack_.back()->getName();
    }
    
    std::shared_ptr<ShaderProgram> shaderProgram = shaderService_->getShaderProgram(program);
    
    // TODO: Assert if shader does not exist
    shaderProgramStack_.push_back(shaderProgram);
    
    if (currShader != program) {
        std::shared_ptr<ShaderProgramOpenGLES2> glProgram = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderProgramStack_.back());
        
        glUseProgram(glProgram->getProgram());
    }
    
    return shaderProgramStack_.back();
}

std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::popShaderProgram()
{
    if (!shaderProgramStack_.empty()) {
        std::string currShader = shaderProgramStack_.back()->getName();
        
        shaderProgramStack_.pop_back();
        
        if (!shaderProgramStack_.empty()) {
            std::shared_ptr<ShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderProgramStack_.back());
            
            if (currShader != program->getName()) {
                // We have a different shader
                glUseProgram(program->getProgram());
            }
        }
    }
    
    return shaderProgramStack_.back();
}

void BGE::RenderServiceOpenGLES2::drawRect(Vector2 &position, Vector2 &size, Vector4 &color)
{
    VertexColor vertices[4];
    
    if (hasInvertedYAxis()) {
        /*
         
            +Y down
         
            0 ------- 1
            | \      |
            |  \     |
            |   \    |
            |    \   |
            |     \  |
            |      \ |
            |       \|
            3--------2
         
            0 - 1 - 2
            0 - 2 - 3
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        vertices[0].position.x = position.x;
        vertices[0].position.y = position.y + size.h;
        vertices[0].position.z = 0;
        vertices[0].color = color;
        
        vertices[1].position.x = position.x + size.w;
        vertices[1].position.y = position.y + size.h;
        vertices[1].position.z = 0;
        vertices[1].color = color;
        
        vertices[2].position.x = position.x + size.w;
        vertices[2].position.y = position.y ;
        vertices[2].position.z = 0;
        vertices[2].color = color;
        
        vertices[3].position.x = position.x;
        vertices[3].position.y = position.y;
        vertices[3].position.z = 0;
        vertices[3].color = color;
    } else {
        /*
         
         +Y up
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 2 - 1
         0 -
         */
        
        
        vertices[0].position.x = position.x;
        vertices[0].position.y = position.y;
        vertices[0].position.z = 0;
        vertices[0].color = color;
        
        vertices[1].position.x = position.x + size.w;
        vertices[1].position.y = position.y;
        vertices[1].position.z = 0;
        vertices[1].color = color;
        
        vertices[2].position.x = position.x + size.w;
        vertices[2].position.y = position.y + size.h;
        vertices[2].position.z = 0;
        vertices[2].color = color;
        
        vertices[3].position.x = position.x;
        vertices[3].position.y = position.y + size.h;
        vertices[3].position.z = 0;
        vertices[3].color = color;
    }

    bool shaderChanged;
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(SimpleShaderProgramId, shaderChanged));

    GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
    GLint colorLocation = glShader->locationForAttribute(SourceColorShaderAttributeId);
    
    // This doesn't use VBO/IBO
    disableVboIbo();
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                   GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
    ++numRectsDrawn_;
    ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
}

void BGE::RenderServiceOpenGLES2::drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4])
{
    VertexColor vertices[4];
    
    if (hasInvertedYAxis()) {
        /*
         
         +Y down
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 1 - 2
         0 - 2 - 3
         
         NOTE: Should render using CCW. The convention normally seems index 0 in lower left corner going CCW.
         
         */
        
        vertices[0].position.x = position.x;
        vertices[0].position.y = position.y + size.h;
        vertices[0].position.z = 0;
        vertices[0].color = color[0];
        
        vertices[1].position.x = position.x + size.w;
        vertices[1].position.y = position.y + size.h;
        vertices[1].position.z = 0;
        vertices[1].color = color[1];
        
        vertices[2].position.x = position.x + size.w;
        vertices[2].position.y = position.y ;
        vertices[2].position.z = 0;
        vertices[2].color = color[2];
        
        vertices[3].position.x = position.x;
        vertices[3].position.y = position.y;
        vertices[3].position.z = 0;
        vertices[3].color = color[3];
    } else {
        /*
         
         +Y up
         
         0 ------- 1
         | \      |
         |  \     |
         |   \    |
         |    \   |
         |     \  |
         |      \ |
         |       \|
         3--------2
         
         0 - 2 - 1
         0 -
         */
        
        vertices[0].position.x = position.x;
        vertices[0].position.y = position.y;
        vertices[0].position.z = 0;
        vertices[0].color = color[0];
        
        vertices[1].position.x = position.x + size.w;
        vertices[1].position.y = position.y;
        vertices[1].position.z = 0;
        vertices[1].color = color[1];
        
        vertices[2].position.x = position.x + size.w;
        vertices[2].position.y = position.y + size.h;
        vertices[2].position.z = 0;
        vertices[2].color = color[2];
        
        vertices[3].position.x = position.x;
        vertices[3].position.y = position.y + size.h;
        vertices[3].position.z = 0;
        vertices[3].color = color[3];
    }
    
    bool shaderChanged;
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(SimpleShaderProgramId, shaderChanged));
    
    GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
    GLint colorLocation = glShader->locationForAttribute(SourceColorShaderAttributeId);
    
    // This doesn't use VBO/IBO
    disableVboIbo();

    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                   GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
    ++numRectsDrawn_;
    ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
}

void BGE::RenderServiceOpenGLES2::drawTexture(Vector2 &position, std::shared_ptr<Texture> texture)
{
    if (texture) {
        VertexTex vertices[4];

        if (texture && texture->isValid()) {
            const Vector2 *xys = texture->getXYs();
            const Vector2 *uvs = texture->getUVs();
            
            vertices[0].position.x = position.x + xys[0].x;
            vertices[0].position.y = position.y + xys[0].y;
            vertices[0].position.z = 0;
            vertices[0].tex.x = uvs[0].x;
            vertices[0].tex.y = uvs[0].y;
            
            vertices[1].position.x = position.x + xys[1].x;
            vertices[1].position.y = position.y + xys[1].y;
            vertices[1].position.z = 0;
            vertices[1].tex.x = uvs[1].x;
            vertices[1].tex.y = uvs[1].y;
            
            vertices[2].position.x = position.x + xys[2].x;
            vertices[2].position.y = position.y + xys[2].y;
            vertices[2].position.z = 0;
            vertices[2].tex.x = uvs[2].x;
            vertices[2].tex.y = uvs[2].y;
            
            vertices[3].position.x = position.x + xys[3].x;
            vertices[3].position.y = position.y + xys[3].y;
            vertices[3].position.z = 0;
            vertices[3].tex.x = uvs[3].x;
            vertices[3].tex.y = uvs[3].y;

            bool shaderChanged;
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(TextureShaderProgramId, shaderChanged));
            
            GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
        
            setBlend(true);
            setBlendFunc(BlendFunc::Src_ONE_Dst_ONE_MINUS_SRC_ALPHA);
            setTexture(texture->getTarget(), texture->getHWTextureId());
            
            GLint texCoordLocation = glShader->locationForAttribute(TexCoordInShaderAttributeId);
            GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);

            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(texCoordLocation);

            // This doesn't use VBO/IBO
            disableVboIbo();
            
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
            
            glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                           GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
            ++numSpritesDrawn_; // Treated like a sprite
            ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawFlatRect(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto flatRect = gameObject->getComponentLockless<FlatRectRenderComponent>(space);
        
        if (flatRect) {
            Vertex *const vertices = flatRect->getVertices();
            auto material = flatRect->getMaterialLockless();
            
            if (material) {
                bool shaderChanged;
                std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(LineShaderProgramId, shaderChanged));

                GLint colorLocation = glShader->locationForUniform(ColorShaderUniformId);
                GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
                
                Color color;
                material->getColor(color);
                glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);

                if (transform) {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
                } else {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
                }

                setBlend(true);
                setBlendFunc(BlendFunc::Src_SRC_ALPHA_Dst_ONE_MINUS_SRC_ALPHA);
                
                GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);

                // This doesn't use VBO/IBO
                disableVboIbo();
                
                glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0]);

                glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                               GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
                ++numRectsDrawn_;
                ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawMaskRect(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto maskRect = gameObject->getComponentLockless<MaskComponent>(space);
        
        if (maskRect) {
            Vertex *const vertices = maskRect->getVertices();
            
            auto material = maskRect->getMaterialLockless();
            
            if (material) {
                bool shaderChanged;
                std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(LineShaderProgramId, shaderChanged));
                
                GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
                GLint colorLocation = glShader->locationForUniform(ColorShaderUniformId);

                if (transform) {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
                } else {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
                }
                
                GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
                glEnableVertexAttribArray(positionLocation);
                
                // This doesn't use VBO/IBO
                disableVboIbo();

                glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                      sizeof(Vertex), &vertices[0]);
                
                Color color;
                material->getColor(color);
                glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);
                
                glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                               GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
                ++numMasksDrawn_;
                ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawTextureMask(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto mask = gameObject->getComponentLockless<TextureMaskComponent>(space);
        
        if (mask) {
            VertexTex * vertices = mask->getVertices();
            auto material = mask->getMaterialLockless();
            if (material) {
                auto textureHandle = material->getTextureHandle();
                auto texture = Game::getInstance()->getTextureService()->getTextureLockless(textureHandle);
                
                if (texture) {
                    if (texture && texture->isValid()) {
                        bool shaderChanged;
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(TextureShaderProgramId, shaderChanged));
                        
                        GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
                        
                        if (transform) {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
                        } else {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
                        }
                        
                        setBlend(false);
                        setTexture(texture->getTarget(), texture->getHWTextureId());
                        
                        GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
                        GLint texCoordLocation = glShader->locationForAttribute(TexCoordInShaderAttributeId);

                        glEnableVertexAttribArray(positionLocation);
                        glEnableVertexAttribArray(texCoordLocation);
                        
                        // This doesn't use VBO/IBO
                        disableVboIbo();
                        
                        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), &vertices[0]);
                        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));

                        glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                                       GL_UNSIGNED_BYTE, &VertexIndices[0]);
#ifdef SUPPORT_PROFILING
                        ++numMasksDrawn_;
                        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
                    }
                }
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawDebugQuads(std::vector<Vector3> points, Color &color) {
    bool shaderChanged;
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(LineShaderProgramId, shaderChanged));
    
    GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
    GLint colorLocation = glShader->locationForUniform(ColorShaderUniformId);
    
    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
    
    glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);

    setLineWidth(2);
    
    GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
    glEnableVertexAttribArray(positionLocation);

    // This doesn't use VBO/IBO
    disableVboIbo();

    auto numPoints = points.size();
    for (size_t index=0;index<numPoints;index += 4) {
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), &points[index]);
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) 4);
#ifdef SUPPORT_PROFILING
        ++numRectsDrawn_;
        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
    }
}

void BGE::RenderServiceOpenGLES2::drawLines(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto line = gameObject->getComponentLockless<LineRenderComponent>(space);
        
        if (line) {
            auto material = line->getMaterialLockless();
            const auto& points = line->getPoints();
            Vector3 vertices[points.size()];

            uint32_t index = 0;
            
            for (auto const &pt : points) {
                vertices[index].x = pt.x;
                vertices[index].y = pt.y;
                vertices[index].z = 0;
                
                index++;
            }
            
            bool shaderChanged;
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(LineShaderProgramId, shaderChanged));
            
            GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
            GLint colorLocation = glShader->locationForUniform(ColorShaderUniformId);

            Color color;
            material->getColor(color);
            glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);

            if (transform) {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
            } else {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
            }
            
            setLineWidth(line->getThickness());
            
            GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
            glEnableVertexAttribArray(positionLocation);

            // This doesn't use VBO/IBO
            disableVboIbo();
            
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), &vertices[0]);
            
            GLenum mode;
            if (line->isLineLoop()) {
                mode = GL_LINE_LOOP;
            } else {
                mode = GL_LINE_STRIP;
            }
            
            glDrawArrays(mode, 0, (GLsizei) points.size());
#ifdef SUPPORT_PROFILING
            ++numLinesDrawn_;
            ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawPolyLines(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto line = gameObject->getComponentLockless<PolyLineRenderComponent>(space);
        
        if (line) {
            const auto& points = line->getPoints();
            const auto& colors = line->getColors();
            
            // Convert our data to what VASEr needs
            auto numColors = colors.size();
            VASEr::Vec2 *vPoints = new VASEr::Vec2[points.size()];
            VASEr::Color *vColors = new VASEr::Color[points.size()];
            
            auto numPoints = points.size();
            assert(numPoints == numColors || numColors == 1);
            
            for (size_t i=0;i<numPoints;++i) {
                auto colorIndex = i;
                vPoints[i].x = points[i].x;
                vPoints[i].y = points[i].y;
                
                if (numColors == 1) {
                    colorIndex = 0;
                }
                vColors[i].r = colors[colorIndex].r;
                vColors[i].g = colors[colorIndex].g;
                vColors[i].b = colors[colorIndex].b;
                vColors[i].a = colors[colorIndex].a;
            }
            
            VASEr::polyline_opt opt;
            
            switch (line->getJoint()) {
                case PolyLineRenderComponent::Joint::bevel:
                    opt.joint = VASEr::PLJ_bevel;
                    break;
                case PolyLineRenderComponent::Joint::round:
                    opt.joint = VASEr::PLJ_round;
                    break;
                default:
                    opt.joint = VASEr::PLJ_miter;
                    break;
            }
            
            switch (line->getCap()) {
                case PolyLineRenderComponent::Cap::butt:
                    opt.cap = VASEr::PLC_butt;
                    break;
                case PolyLineRenderComponent::Cap::round:
                    opt.cap = VASEr::PLC_round;
                    break;
                case PolyLineRenderComponent::Cap::square:
                    opt.cap = VASEr::PLC_square;
                    break;
                case PolyLineRenderComponent::Cap::rect:
                    opt.cap = VASEr::PLC_rect;
                    break;
                case PolyLineRenderComponent::Cap::both:
                    opt.cap = VASEr::PLC_both;
                    break;
                case PolyLineRenderComponent::Cap::first:
                    opt.cap = VASEr::PLC_first;
                    break;
                case PolyLineRenderComponent::Cap::last:
                    opt.cap = VASEr::PLC_last;
                    break;
                case PolyLineRenderComponent::Cap::none:
                    opt.cap = VASEr::PLC_none;
                    break;
            }
            
            opt.tess = nullptr;
            opt.feather = line->isFeather();
            opt.feathering = line->getFeathering();
            opt.no_feather_at_cap = line->getNoFeatherAtCap();
            opt.no_feather_at_core = line->getNoFeatherAtCore();
            
            bool shaderChanged;
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(PolyLineShaderProgramId, shaderChanged));
            GLint colorLocation = glShader->locationForAttribute(SourceColorShaderAttributeId);
            GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
            
            if (transform) {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
            } else {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
            }

            setBlend(true);
            setBlendFunc(BlendFunc::Src_SRC_ALPHA_Dst_ONE_MINUS_SRC_ALPHA);
            
            GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
            glEnableVertexAttribArray(positionLocation);

            // This doesn't use VBO/IBO
            disableVboIbo();
            
            VASEr::VASErin::backend::set_uniforms(positionLocation, colorLocation);

            VASEr::polyline(vPoints, vColors, line->getThickness(), static_cast<int>(points.size()), &opt);
            
            if (vPoints) {
                delete [] vPoints;
            }
            if (vColors) {
                delete [] vColors;
            }
#ifdef SUPPORT_PROFILING
            ++numPolylinesDrawn_;
            ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawSprite(Space *space, GameObject *gameObject, TransformComponent *transform) {
    if (gameObject) {
        auto sprite = gameObject->getComponentLockless<SpriteRenderComponent>(space);
        
        if (sprite) {
            VertexTex * vertices = sprite->getVertices();
            auto material = sprite->getMaterialLockless();
            if (material) {
                auto textureHandle = material->getTextureHandle();
                auto texture = Game::getInstance()->getTextureService()->getTextureLockless(textureHandle);
                
                if (texture) {
#ifdef NOT_YET
                    NSLog(@"Rendering sprite %s", texture->getName().c_str());
#endif
                    
                    if (texture && texture->isValid()) {
                        bool shaderChanged;
#if 0
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(ColorMatrixTextureShaderProgramId, shaderChanged));
#else
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(FullColorTextureShaderProgramId, shaderChanged));
#endif
                        
                        GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
                        GLint colorMatrixLocation = glShader->locationForUniform(ColorMatrixShaderUniformId);
                        GLint colorMatOffsetLocation = glShader->locationForUniform(ColorMatOffsetShaderUniformId);
                        auto colorMultiplierLocation = glShader->locationForUniform(ColorMultiplierShaderUniformId);
                        auto colorOffsetLocation = glShader->locationForUniform(ColorOffsetShaderUniformId);
                        
                        if (transform) {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transform->worldMatrix_.m);
                        } else {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) identifyMatrix_.m);
                        }

                        glUniformMatrix4fv(colorMatrixLocation, 1, 0, (GLfloat *) currentColorMatrix_.matrix.m);
                        glUniform4fv(colorMatOffsetLocation, 1, (GLfloat *) currentColorMatrix_.offset.v);

                        glUniform4fv(colorMultiplierLocation, 1, (GLfloat *) currentColorTransform_.multiplier.v);
                        glUniform4fv(colorOffsetLocation, 1, (GLfloat *) currentColorTransform_.offset.v);

                        setBlend(false);
                        setTexture(texture->getTarget(), texture->getHWTextureId());

                        GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
                        GLint texCoordLocation = glShader->locationForAttribute(TexCoordInShaderAttributeId);
                        glEnableVertexAttribArray(positionLocation);
                        glEnableVertexAttribArray(texCoordLocation);
                        
                        auto vboId = texture->getHWVboId();
                        if (vboId) {
                            setVbo(texture->getHWVboId());
#ifdef USE_IBO
                            auto iboId = texture->getHWIboId();
                            if (iboId) {
                                setIbo(iboId);
                            } else {
                                if (setIbo(indexBufferId_)) {
                                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, texture->getVboIndicesSize(), texture->getVboIndices(), GL_DYNAMIC_DRAW);
                                }
                            }
#else
                            setIbo(0);
#endif
                            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                                  sizeof(VertexTex), 0);
                            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                                  sizeof(VertexTex), (void *) sizeof(Vector3));
#ifdef USE_IBO
                            glDrawElements(GL_TRIANGLES, texture->getVboIndicesCount(), texture->getVboIndexType(), 0);
#else
                            glDrawElements(GL_TRIANGLES, texture->getVboIndicesCount(), texture->getVboIndexType(), texture->getVboIndices());
#endif
                        } else {
                            // This doesn't use VBO/IBO
                            disableVboIbo();

                            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                                  sizeof(VertexTex), &vertices[0]);
                            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                                  sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                            glDrawElements(GL_TRIANGLES, sizeof(VertexIndices)/sizeof(VertexIndices[0]),
                                           GL_UNSIGNED_BYTE, &VertexIndices[0]);
                        }
                        
#ifdef SUPPORT_PROFILING
                        ++numSpritesDrawn_;
                        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
                    }
                }
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawString(Space *space, TextComponent *text, Font *font, TransformComponent *transform, ColorMatrix& colorMatrix, ColorTransform& colorTransform, bool minimum) {
    if (text->isMultiline()) {
        auto multiText = text->getMultiText();
        auto yPos = text->getMultiTextY();

        if (text->isDropShadow()) {
            auto off = text->getDropShadowOffset();
            drawString(space, multiText, font, off.x, off.y, yPos, text->getBoundsWidth(), transform, (Color&) text->getDropShadowColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
        }

        drawString(space, multiText, font, 0, 0, yPos, text->getBoundsWidth(), transform, (Color&) text->getColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
    } else {
        if (text->isDropShadow()) {
            auto off = text->getDropShadowOffset();
            drawString(space, text->getText(), font, transform->getWorldMatrixRaw(), text->getBoundsWidth(), off.x, off.y, (Color&) text->getDropShadowColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
        }

        drawString(space, text->getText(), font, transform->getWorldMatrixRaw(), text->getBoundsWidth(), 0, 0, (Color&) text->getColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
    }
}

void BGE::RenderServiceOpenGLES2::drawString(Space *space, std::vector<std::string> &strs, Font *font, float xOffset, float yOffset, std::vector<float> &yPos, float defWidth, TransformComponent *transform, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    const float *rawMatrix = transform->getWorldMatrixRaw();
    auto index = 0;

    for (auto &str : strs) {
        float yOff = yPos[index] + yOffset;

        drawString(space, str, font, rawMatrix, defWidth, xOffset, yOff, color, colorMatrix, colorTransform, horizAlignment, vertAlignment, minimum);

        index++;
    }
}

#define RENDER_STRING_WITH_GL_ARRAY

void BGE::RenderServiceOpenGLES2::drawString(Space *space, std::string str, Font *font, const float *rawMatrix, float defWidth, float xOffset, float yOffset, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    auto textureService = Game::getInstance()->getTextureService();
    auto textureAtlas = textureService->getTextureAtlasLockless(font->getTextureAtlasHandle());

    if (str.length() > 0 && textureAtlas && textureAtlas->isValid()) {
        GLushort indexPtr = 0;
        float x = xOffset;
        float y = yOffset;
        
        // TODO: Adjustment based on alignment to be done here

        stringVertexData_.clear();
        stringIndices_.clear();
        
        const char *chars = str.c_str();
        FontGlyph glyph;
        uint16_t code;
        size_t length = str.length();

        // Compute the offsets if needed
        switch (horizAlignment) {
            case FontHorizontalAlignment::Center:
                x -= font->getStringWidth(str, minimum) / 2.0;
                break;

            case FontHorizontalAlignment::Right:
                x -= font->getStringWidth(str, minimum) - defWidth / 2.0;
                break;

            default:
                x -= defWidth / 2.0;
                break;
        }

        float deltaY = 0;

        switch (vertAlignment) {
            case FontVerticalAlignment::Center:
                deltaY = font->getHeight() / 2.0;
                break;

            case FontVerticalAlignment::Bottom:
                deltaY = font->getHeight();
                break;

            case FontVerticalAlignment::Baseline:
                deltaY = font->getBaseline();
                break;

            default:
                break;
        }

        y -= deltaY;

        float gridX = x;
        float gridY = y;

        uint16_t prev = 0;

        for (size_t i=0;i<length;i++) {
            code = chars[i];

            auto glyph = font->glyphs_.find(code);

            if (glyph != font->glyphs_.end()) {
                if (i == 0 && !minimum) {
                    x += glyph->second.getOffsetX();
                } else {
                    x += glyph->second.getOffsetX();
                }

                if (font->hasKerning() && prev) {
                    int32_t kerning = font->kerningForPair(prev, code);
                    x += kerning;
                    gridX += kerning;
                }

                y += glyph->second.getOffsetY();

                auto texHandle = glyph->second.getTextureHandle();
                auto tex = textureService->getTextureLockless(texHandle);

                if (tex) {
                    const Vector2 *xys = tex->getXYs();
                    const Vector2 *uvs = tex->getUVs();

                    auto numIndices = sizeof(VertexIndices)/sizeof(VertexIndices[0]);
#ifdef RENDER_STRING_WITH_GL_ARRAY
                    for (size_t i=0;i<numIndices;++i) {
                        auto index = VertexIndices[i];
                        VertexTex vtex{{{x + xys[index].x, y + xys[index].y, 0.0}}, {{uvs[index].x, uvs[index].y}}};
                        stringVertexData_.push_back(vtex);
                        stringIndices_.push_back(indexPtr++);
                    }
#else
                    VertexTex vtex{{{x + xys[0].x, y + xys[0].y, 0.0}}, {{uvs[0].x, uvs[0].y}}};
                    stringVertexData_.push_back(vtex);
                    vtex = VertexTex{{{x + xys[1].x, y + xys[1].y, 0.0}}, {{uvs[1].x, uvs[1].y}}};
                    stringVertexData_.push_back(vtex);
                    vtex = VertexTex{{{x + xys[2].x, y + xys[2].y, 0.0}}, {{uvs[2].x, uvs[2].y}}};
                    stringVertexData_.push_back(vtex);
                    vtex = VertexTex{{{x + xys[3].x, y + xys[3].y, 0.0}}, {{uvs[3].x, uvs[3].y}}};
                    stringVertexData_.push_back(vtex);
                    for (size_t i=0;i<numIndices;++i) {
                        stringIndices_.push_back(indexPtr + VertexIndices[i]);
                    }
                    indexPtr += 4;
#endif
#ifdef SUPPORT_PROFILING
                    ++numFontCharactersDrawn_;
#endif /* SUPPORT_PROFILING */
                }

                gridX += glyph->second.getAdvance();
                x = gridX;
                y = gridY;
            }

            prev = code;
        }
        
        // Draw here
        textureAtlas = textureService->getTextureAtlasLockless(font->getTextureAtlasHandle()); // Refresh
        if (textureAtlas) {
            bool shaderChanged;
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram(FullColorFontShaderProgramId, shaderChanged));
            GLint modelLocation = glShader->locationForUniform(ModelViewShaderUniformId);
            GLint colorUniform = glShader->locationForUniform(SourceColorShaderUniformId);
            GLint colorMatrixLocation = glShader->locationForUniform(ColorMatrixShaderUniformId);
            GLint colorMatOffsetLocation = glShader->locationForUniform(ColorMatOffsetShaderUniformId);
            auto colorMultiplierLocation = glShader->locationForUniform(ColorMultiplierShaderUniformId);
            auto colorOffsetLocation = glShader->locationForUniform(ColorOffsetShaderUniformId);
            
            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) rawMatrix);
            
            glUniformMatrix4fv(colorMatrixLocation, 1, 0, (GLfloat *) colorMatrix.matrix.m);
            glUniform4fv(colorMatOffsetLocation, 1, (GLfloat *) colorMatrix.offset.v);
            glUniform4fv(colorMultiplierLocation, 1, (GLfloat *) colorTransform.multiplier.v);
            glUniform4fv(colorOffsetLocation, 1, (GLfloat *) colorTransform.offset.v);
            glUniform4f(colorUniform, color.r, color.g, color.b, color.a);

            setBlend(false);
            setTexture(textureAtlas->getTarget(), textureAtlas->getHWTextureId());
            
            GLint positionLocation = glShader->locationForAttribute(PositionShaderAttributeId);
            GLint texCoordLocation = glShader->locationForAttribute(TexCoordInShaderAttributeId);
            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(texCoordLocation);

            // This doesn't use VBO/IBO
            disableVboIbo();
            
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), &stringVertexData_[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), (GLvoid*) (&stringVertexData_[0].tex));

#ifdef RENDER_STRING_WITH_GL_ARRAY
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(stringVertexData_.size()));
#else
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(stringIndices_.size()),
                           GL_UNSIGNED_SHORT, &stringIndices_[0]);
#endif
#ifdef SUPPORT_PROFILING
            ++numFontCharactersDrawn_;
            ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
        }
    }
}


uint8_t BGE::RenderServiceOpenGLES2::enableMask(Space *space, GameObject *gameObject, TransformComponent *transform) {
    uint8_t maskValue = 0;
    
    if (this->activeMasks_ != 0xFF) {
        
        for (auto i=0;i<MaxActiveMasks;i++) {
            if (!(this->activeMasks_ & MaskIdToMaskValue[i])) {
                maskValue = MaskIdToMaskValue[i];
                break;
            }
        }
        
        if (maskValue) {
            this->activeMasks_ |= maskValue;

            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, maskValue, maskValue);
            glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
            glStencilMask(maskValue);
            
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            
            if (gameObject->hasComponent<MaskComponent>()) {
                drawMaskRect(space, gameObject, transform);
            } else if (gameObject->hasComponent<TextureMaskComponent>()) {
                drawTextureMask(space, gameObject, transform);
            }
      
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glStencilFunc(GL_EQUAL, this->activeMasks_, this->activeMasks_);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }
    }
    
    return maskValue;
}

void BGE::RenderServiceOpenGLES2::disableMask(uint8_t maskBits)
{
    if (maskBits) {
        glStencilMask(maskBits);
        glClear(GL_STENCIL_BUFFER_BIT);
        
        activeMasks_ &= ~maskBits;
        glStencilFunc(GL_EQUAL, this->activeMasks_, this->activeMasks_);
    }
    
    if (!activeMasks_) {
        glDisable(GL_STENCIL_TEST);
    }
}

void BGE::RenderServiceOpenGLES2::queueRender(__attribute__ ((unused)) double time) {
    if (trylock()) {
        if (!isBackgrounded()) {
            [this->getRenderWindow()->getView() display];
        }
        
        unlock();
    }
}

void BGE::RenderServiceOpenGLES2::render()
{
    lock();
    handleServicesLock();

    if (isBackgrounded()) {
        handleServicesUnlock();
        unlock();
        return;
    }

#ifdef SUPPORT_PROFILING
    auto startTime = profiling::EpochTime::timeInMicroSec();

    resetProfilingStats();
#endif /* SUPPORT_PROFILING */

    std::shared_ptr<RenderContextOpenGLES2> glContext = std::dynamic_pointer_cast<RenderContextOpenGLES2>(getRenderContext());
    auto bkgColor = getBackgroundColor();
    
    glClearColor(bkgColor.r, bkgColor.g, bkgColor.b, bkgColor.a);
    
    glStencilMask(0x7f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    this->activeMasks_ = 0;
    
    if (isReady()) {
        assert(colorMatrixStack_.size() == 0);
        assert(colorTransformStack_.size() == 0);
        
        ColorMatrixMakeIdentity(currentColorMatrix_);
        colorMatrixStack_.clear();  // TODO: remove?
        
        ColorTransformMakeIdentity(currentColorTransform_);
        colorTransformStack_.clear();
        
        // Reset render states
        shaderProgramStack_.clear();
        
//        NSLog(@"%f %f", this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        
        // TODO: Content scale factor
//        glViewport(0, 0, this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        auto window = this->getRenderWindow();
        auto view = window->getRenderView(RenderWindow::DefaultRenderViewName);
        glViewport(0, 0, view->getWidth() * this->getRenderWindow()->getContentScaleFactor(), view->getHeight() * window->getContentScaleFactor());
        
        mappedProjectionMatrix_ = projectionMatrix_;
        Matrix4Scale(mappedProjectionMatrix_, window->getToMappedXScale(), window->getToMappedYScale(), 1.0);
        
        auto spaceService = Game::getInstance()->getSpaceService();
        spaceService->getSpaces(spaceHandles_);
        
        for (auto const &handle : spaceHandles_) {
            auto space = spaceService->getSpace(handle);
            
            if (space && space->isActive() && space->isVisible()) {
                space->getRootGameObjects(rootGameObjects_);
                for (auto const &obj : rootGameObjects_) {
                    renderGameObject(obj, space, 0);
                }
            }
        }
        
        if (Game::getInstance()->showCollisionRects()) {
            // Draw collision rects if needed
            Game::getInstance()->getInputService()->getBboxPoints(boundingBoxPoints_, scaledBoundingBoxPoints_);
            Color color = Color{{ 1, 1, 1, 1 }};
            drawDebugQuads(boundingBoxPoints_, color);
            
            // First draw normal bounds
            color = Color{{ 1, 1, 0, 1 }};
            drawDebugQuads(scaledBoundingBoxPoints_, color);
        }
        
        [glContext->getContext() presentRenderbuffer:GL_RENDERBUFFER];
    }

#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
    frameRateCalculator_.nextFrame();
#endif /* SUPPORT_PROFILING */
    handleServicesUnlock();
    unlock();
}

int8_t BGE::RenderServiceOpenGLES2::renderGameObject(GameObject *gameObj, Space *space, uint32_t depth, bool hasNextSibling) {
    uint8_t maskValue = 0;

#ifdef SUPPORT_PROFILING
    ++numGameObjectsIgnored_;
    ++numProcessedObjects_;
#endif /* SUPPORT_PROFILING */

    if (!gameObj->isActive()) {
        return maskValue;
    }
    
    // TODO: Transform
    auto transformComponent = gameObj->getComponentLockless<TransformComponent>(space);
    auto root = depth == 0;
    
    if (transformComponent) {
        if (!transformComponent->canRender()) {
            return maskValue;
        }

        auto parent = transformComponent->getParentLockless(space);

        if (root && parent) {
            return maskValue;
        } else if (!root && !parent) {
            return maskValue;
        }

        // Need to update matrix
        transformComponent->updateMatrixLockless(space);

        // Since we have the transform, push our
        pushColorMatrix();
        pushColorTransform();
        
        auto colorMatrix = gameObj->getComponentLockless<ColorMatrixComponent>(space);
        auto colorTransform = gameObj->getComponentLockless<ColorTransformComponent>(space);
        
        if (colorMatrix) {
            auto tMat = currentColorMatrix_ * colorMatrix->matrix;
            currentColorMatrix_ = tMat;
        }
        
        if (colorTransform) {
            auto tColor = currentColorTransform_ * colorTransform->transform;
            currentColorTransform_ = tColor;
        }
        
        if (gameObj->hasComponent<SpriteRenderComponent>()) {
            drawSprite(space, gameObj, transformComponent);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<TextComponent>()) {
            auto text = gameObj->getComponentLockless<TextComponent>(space);
            Font *font = Game::getInstance()->getFontService()->getFont(text->getFontHandle());
            
            if (font) {
                drawString(space, text, font, transformComponent, currentColorMatrix_, currentColorTransform_);
            }
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<PolyLineRenderComponent>()) {
            drawPolyLines(space, gameObj, transformComponent);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<LineRenderComponent>()) {
            drawLines(space, gameObj, transformComponent);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<FlatRectRenderComponent>()) {
            drawFlatRect(space, gameObj, transformComponent);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<MaskComponent>() || gameObj->hasComponent<TextureMaskComponent>()) {
            maskValue = enableMask(space, gameObj, transformComponent);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        }

        uint8_t childrenMasks = 0;;
        
        while (depth >= orderedChildrenHandles_.size()) {
            orderedChildrenHandles_.push_back(std::vector<TransformComponentHandle>());
        }
        
        // Determine if we have children, if we do process them.
        transformComponent->getOrderedChildrenHandles(orderedChildrenHandles_[depth]);
        auto length = orderedChildrenHandles_[depth].size();

        for (size_t i=0;i<length;++i) {
            auto& childrenHandles = orderedChildrenHandles_[depth];
            auto childXformHandle = childrenHandles[i];
            auto childXform = componentService_->getComponentLockless<TransformComponent>(childXformHandle.getHandle());
            
            if (childXform->hasGameObject()) {
                auto childObj = childXform->getGameObjectLockless(space);
                
                if (childObj) {
                    childrenMasks |= renderGameObject(childObj, space, depth + 1, i < (length - 1));
                }
            }
        }
        
        if (childrenMasks) {
            disableMask(childrenMasks);
        }
        
        if (!hasNextSibling) {
            disableMask(maskValue);
            maskValue = 0;
        }
        
        // First sort
        
        popColorTransform();
        popColorMatrix();
    }
    
    return maskValue;
}

void BGE::RenderServiceOpenGLES2::pushColorMatrix() {
    colorMatrixStack_.push_back(currentColorMatrix_);
}

void BGE::RenderServiceOpenGLES2::popColorMatrix() {
    currentColorMatrix_ = colorMatrixStack_.back();
    colorMatrixStack_.pop_back();
}

void BGE::RenderServiceOpenGLES2::pushColorTransform() {
    colorTransformStack_.push_back(currentColorTransform_);
}

void BGE::RenderServiceOpenGLES2::popColorTransform() {
    currentColorTransform_ = colorTransformStack_.back();
    colorTransformStack_.pop_back();
}

bool BGE::RenderServiceOpenGLES2::setTexture(GLenum target, GLuint texId) {
    if (currentTextureId_ != texId) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(target, texId);
        currentTextureId_ = texId;
#ifdef SUPPORT_PROFILING
        ++numTexturesChanged_;
#endif /* SUPPORT_PROFILING */
        return true;
    }
    return false;
}

bool BGE::RenderServiceOpenGLES2::setBlend(bool blend) {
    if (currentBlend_ != blend) {
        currentBlend_ = blend;
        if (blend) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
#ifdef SUPPORT_PROFILING
        ++numBlendChanged_;
#endif /* SUPPORT_PROFILING */
        return true;
    }
    return false;
}

bool BGE::RenderServiceOpenGLES2::setBlendFunc(BlendFunc blendFunc) {
    if (currentBlendFunc_ != blendFunc) {
        currentBlendFunc_ = blendFunc;
        switch (blendFunc) {
            case BlendFunc::Src_ONE_Dst_ONE_MINUS_SRC_ALPHA:
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendFunc::Src_SRC_ALPHA_Dst_ONE_MINUS_SRC_ALPHA:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            default:
                glBlendFunc(GL_ONE, GL_ZERO);
                break;
        }
#ifdef SUPPORT_PROFILING
        ++numBlendFuncChanged_;
#endif /* SUPPORT_PROFILING */
        return true;
    }
    return false;
}

bool BGE::RenderServiceOpenGLES2::setLineWidth(GLfloat width) {
    if (currentLineWidth_ != width) {
        currentLineWidth_ = width;
        glLineWidth(width);
        return true;
    }
    return false;
}

bool BGE::RenderServiceOpenGLES2::setVbo(GLuint vbo) {
    if (currentVbo_ != vbo) {
        currentVbo_ = vbo;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        return true;
    }
    return false;
}

bool BGE::RenderServiceOpenGLES2::setIbo(GLuint ibo) {
    if (currentIbo_ != ibo) {
        currentIbo_ = ibo;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        return true;
    }
    return false;
}

void BGE::RenderServiceOpenGLES2::disableVboIbo() {
    setVbo(0);
    setIbo(0);
}

void BGE::RenderServiceOpenGLES2::windowMappedDimensionsUpdated(std::shared_ptr<RenderWindow> window) {
    if (window == renderWindow_) {
        auto window = this->getRenderWindow();
        auto view = window->getRenderView(RenderWindow::DefaultRenderViewName);
        glViewport(0, 0, view->getWidth() * this->getRenderWindow()->getContentScaleFactor(), view->getHeight() * window->getContentScaleFactor());
        
        mappedProjectionMatrix_ = projectionMatrix_;
        Matrix4Scale(mappedProjectionMatrix_, window->getToMappedXScale(), window->getToMappedYScale(), 1.0);
        
        // Now notify shaders through Shader Service
        getShaderService()->windowMappedDimensionsUpdated();
    }
}

void BGE::RenderServiceOpenGLES2::threadCleanup() {
    if (indexBufferId_) {
        setIbo(0);
        glDeleteBuffers(1, &indexBufferId_);
        indexBufferId_ = 0;
    }
}


void BGE::RenderServiceOpenGLES2::createTexture(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(item.data);
    GLuint tex;
    GLint alignment = 0;
    auto glFormat = data->glFormat;
    std::shared_ptr<Error> error;
    glGenTextures(1, &tex);
    if (glFormat == GL_RGB) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    
    // Indicate we're the current texture since we did a bind
    currentTextureId_ = tex;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, data->textureWidth, data->textureHeight, 0, glFormat, GL_UNSIGNED_BYTE, data->textureBuffer);
    if (glFormat == GL_RGB) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    }
    GLenum glErr = glGetError();
    if (glErr == GL_NO_ERROR) {
        // currentTextureId_ reflects the last glBindTexture bound
        currentTextureId_ = tex;
        data->glHwId = tex;
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorAllocation);
    }
    if (item.callback) {
        item.callback(item, error);
    }
}

void BGE::RenderServiceOpenGLES2::destroyTexture(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderTextureCommandData>(item.data);
    std::shared_ptr<Error> error;
    GLuint texId = data->glHwId;
    if (texId) {
        if (currentTextureId_ == texId) {
            // If our current texture is being destroyed, reset to no texture
            setTexture(GL_TEXTURE_2D, 0);
        }
        glDeleteTextures(1, &texId);
    }
    if (item.callback) {
        item.callback(item, error);
    }
}

void BGE::RenderServiceOpenGLES2::createVbo(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderVboCommandData>(item.data);
    std::shared_ptr<Error> error;
    GLuint vbo;
    glGenBuffers(1, &vbo);
    
    // set Vbo since we are going to glBufferData
    setVbo(vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTex) * data->numVertexTex, data->vertexTexData, GL_STATIC_DRAW);
    GLenum glErr = glGetError();
    if (glErr == GL_NO_ERROR) {
        data->glVboId = vbo;
        
        if (!indexBufferId_) {
            // We need to create at least one index buffer
            glGenBuffers(1, &indexBufferId_);
        }
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorVboAllocation);
    }
    
    // Unbind vbo
    setVbo(0);
    
    if (item.callback) {
        item.callback(item, error);
    }
}

void BGE::RenderServiceOpenGLES2::destroyVbo(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderVboCommandData>(item.data);
    std::shared_ptr<Error> error;
    GLuint vbo = data->glVboId;
    if (vbo != 0) {
        if (vbo == currentVbo_) {
            // If we're destroying the current Vbo, unbind
            setVbo(0);
        }
        glDeleteBuffers(1, &vbo);
    }
    if (item.callback) {
        item.callback(item, error);
    }
}

void BGE::RenderServiceOpenGLES2::createIbo(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderIboCommandData>(item.data);
    std::shared_ptr<Error> error;
    GLuint ibo;
    glGenBuffers(1, &ibo);
    
    // set Ibo since we are going to glBufferData
    setIbo(ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->numIndices * data->indexSize, data->indices, GL_STATIC_DRAW);
    GLenum glErr = glGetError();
    if (glErr == GL_NO_ERROR) {
        data->glIboId = ibo;
    } else {
        error = std::make_shared<Error>(Texture::ErrorDomain, TextureErrorVboAllocation);
    }
    
    // Unbind ibo
    setIbo(0);
    
    if (item.callback) {
        item.callback(item, error);
    }
}

void BGE::RenderServiceOpenGLES2::destroyIbo(const RenderCommandItem& item) {
    auto data = std::dynamic_pointer_cast<RenderIboCommandData>(item.data);
    std::shared_ptr<Error> error;
    GLuint ibo = data->glIboId;
    if (ibo != 0) {
        if (ibo == currentIbo_) {
            // If we're destroying the current Ibo, unbind
            setIbo(0);
        }
        glDeleteBuffers(1, &ibo);
    }
    if (item.callback) {
        item.callback(item, error);
    }
}

#ifdef SUPPORT_PROFILING

void BGE::RenderServiceOpenGLES2::resetProfilingStats() {
    RenderService::resetProfilingStats();
}

#endif /* SUPPORT_PROFILING */

#endif /* SUPPORT_OPENGLES2 */

