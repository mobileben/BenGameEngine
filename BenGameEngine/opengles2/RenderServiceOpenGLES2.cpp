//
//  RenderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

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

const GLubyte Indices[] = {
    0, 1, 2,
    2, 3, 0
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

BGE::RenderServiceOpenGLES2::RenderServiceOpenGLES2() : activeMasks_(0), currentTextureId_(0) {
    shaderService_ = std::make_shared<ShaderServiceOpenGLES2>();
    ShaderServiceOpenGLES2::mapShaderBundle("BenGameEngineBundle");
    
    Matrix4MakeIdentify(projectionMatrix_);

#ifdef SUPPORT_PROFILING
    resetProfilingStats();
#endif /* SUPPORT_PROFILING */
}

void BGE::RenderServiceOpenGLES2::initialize() {}
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
    auto view = this->getRenderWindow()->getView();
    size_t memory = 0;
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
}

void BGE::RenderServiceOpenGLES2::bindRenderWindow(std::shared_ptr<RenderContext> context, std::shared_ptr<RenderWindow> window)
{
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

#if 1
    setCoordinateSystem2D(Render2DCoordinateSystem::OpenGLCentered);
#else
    setCoordinateSystem2D(Render2DCoordinateSystem::Traditional);
#endif
}

void BGE::RenderServiceOpenGLES2::resizeRenderWindow()
{
}

void BGE::RenderServiceOpenGLES2::createShaders()
{
    std::shared_ptr<Shader> vShader = this->getShaderService()->createShader(ShaderType::Vertex, "SimpleVertex");
    std::shared_ptr<Shader> fShader = this->getShaderService()->createShader(ShaderType::Fragment, "SimpleFragment");
    std::shared_ptr<ShaderProgram> program = this->getShaderService()->createShaderProgram("Default", {vShader,  fShader}, { "Position", "SourceColor" }, { "ModelView", "Projection" });

    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "LineVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "LineFragment");
    program = this->getShaderService()->createShaderProgram("Line", {vShader,  fShader}, { "Position" }, { "ModelView", "Projection", "Color" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "TextureVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "TextureFragment");
    program = this->getShaderService()->createShaderProgram("Texture", {vShader,  fShader}, { "Position", "SourceColor", "TexCoordIn" }, { "ModelView", "Projection", "Texture" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "ColorMatrixTextureVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "ColorMatrixTextureFragment");
    program = this->getShaderService()->createShaderProgram("ColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "ColorMatrix", "ColorMatOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "FullColorTextureVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "FullColorTextureFragment");
    program = this->getShaderService()->createShaderProgram("FullColorTexture", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "ColorMatrix", "ColorMatOffset", "ColorMultiplier", "ColorOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "MaskColorMatrixTextureVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "MaskColorMatrixTextureFragment");
    program = this->getShaderService()->createShaderProgram("MaskColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn", "MaskTexCoordIn" }, { "ModelView", "Projection", "Texture", "MaskTexture", "ColorMatrix", "ColorMatOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "FontVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "FontFragment");
    program = this->getShaderService()->createShaderProgram("Font", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "SourceColor" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "FullColorFontVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "FullColorFontFragment");
    program = this->getShaderService()->createShaderProgram("FullColorFont", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "SourceColor", "ColorMatrix", "ColorMatOffset", "ColorMultiplier", "ColorOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "PolyLineVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "PolyLineFragment");
    program = this->getShaderService()->createShaderProgram("PolyLine", {vShader,  fShader}, { "Position", "SourceColor" }, { "ModelView", "Projection" });
}
std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::useShaderProgram(const std::string& program) {
    auto shaderProgram = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(shaderService_->getShaderProgram(program));
    if (shaderProgram) {
        if (currentShader_ != shaderProgram) {
            glUseProgram(shaderProgram->getProgram());
            currentShader_ = shaderProgram;
#ifdef SUPPORT_PROFILING
            ++numShadersChanged_;
#endif /* SUPPORT_PROFILING */
        }
    }
    return currentShader_;
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
        
        for (auto const &kv: glProgram->getAttributes()) {
            
        }
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
    GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };
    
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

    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
#ifdef SUPPORT_PROFILING
    ++numRectsDrawn_;
    ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
}

void BGE::RenderServiceOpenGLES2::drawShadedRect(Vector2 &position, Vector2 &size, Vector4 color[4])
{
    VertexColor vertices[4];
    GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };
    
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
    
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
#ifdef SUPPORT_PROFILING
    ++numRectsDrawn_;
    ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
}

void BGE::RenderServiceOpenGLES2::drawTexture(Vector2 &position, std::shared_ptr<Texture> texture)
{
    if (texture) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant

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

            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Texture"));
            
            GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
            
            GLint positionLocation = glShader->locationForAttribute("Position");
//            texCoordLocation = glShader->locationForAttribute("TexCoordIn");
            
            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(texCoordLocation);
            
            GLint textureUniform = glShader->locationForUniform("Texture");
            GLint projectionLocation = glShader->locationForUniform("Projection");
            glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
            
            // This is a hack for now

            GLint modelLocation = glShader->locationForUniform("ModelView");
            Matrix4 mat;
            
            Matrix4MakeIdentify(mat);
            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
            // END HACK
        
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            
            glActiveTexture(GL_TEXTURE0);
            setTexture(texture->getTarget(), texture->getHWTextureId());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
            
            glUniform1i(textureUniform, 0);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                           GL_UNSIGNED_BYTE, &indices[0]);
#ifdef SUPPORT_PROFILING
            ++numSpritesDrawn_; // Treated like a sprite
            ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawFlatRect(GameObject *gameObject) {
    if (gameObject) {
        auto flatRect = gameObject->getComponent<FlatRectRenderComponent>();
        
        if (flatRect) {
            Vertex *const vertices = flatRect->getVertices();
            auto material = flatRect->getMaterial();
            
            if (material) {
                std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Line"));

                GLint positionLocation = glShader->locationForAttribute("Position");
                GLint projectionLocation = glShader->locationForUniform("Projection");
                GLint colorLocation = glShader->locationForUniform("Color");
                GLint modelLocation = glShader->locationForUniform("ModelView");
                auto transformComponent = gameObject->getComponent<TransformComponent>();

                glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                      sizeof(Vertex), &vertices[0]);
                
                Color color;
                
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                material->getColor(color);
                glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);

                if (transformComponent) {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transformComponent->worldMatrix_.m);
                } else {
                    // TODO: This is a hack for now
                    Matrix4 mat;
                    
                    Matrix4MakeIdentify(mat);
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
                }

                glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                               GL_UNSIGNED_BYTE, &Indices[0]);
#ifdef SUPPORT_PROFILING
                ++numRectsDrawn_;
                ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawMaskRect(GameObject *gameObject) {
    if (gameObject) {
        auto maskRect = gameObject->getComponent<MaskComponent>();
        
        if (maskRect) {
            Vertex *const vertices = maskRect->getVertices();
            
            auto material = maskRect->getMaterial();
            
            if (material) {
                std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Line"));
                auto transformComponent = gameObject->getComponent<TransformComponent>();
                
                GLint positionLocation = glShader->locationForAttribute("Position");
                GLint projectionLocation = glShader->locationForUniform("Projection");
                GLint modelLocation = glShader->locationForUniform("ModelView");
                GLint colorLocation = glShader->locationForUniform("Color");

                glEnableVertexAttribArray(positionLocation);

                if (transformComponent) {
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transformComponent->worldMatrix_.m);
                } else {
                    // This is a hack for now
                    Matrix4 mat;
                    
                    Matrix4MakeIdentify(mat);
                    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
                }

                glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                      sizeof(Vertex), &vertices[0]);
                
                Color color;

                material->getColor(color);
                glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);
                glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                               GL_UNSIGNED_BYTE, &Indices[0]);
#ifdef SUPPORT_PROFILING
                ++numMasksDrawn_;
                ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawTextureMask(GameObject *gameObject) {
    if (gameObject) {
        auto mask = gameObject->getComponent<TextureMaskComponent>();
        
        if (mask) {
            VertexTex *const vertices = mask->getVertices();
            auto material = mask->getMaterial();
            if (material) {
                auto textureHandle = material->getTextureHandle();
                auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle);
                
                if (texture) {
                    if (texture && texture->isValid()) {
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Texture"));
                        
                        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        GLint positionLocation = glShader->locationForAttribute("Position");
                        //            texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        glEnableVertexAttribArray(positionLocation);
                        glEnableVertexAttribArray(texCoordLocation);
                        
                        GLint textureUniform = glShader->locationForUniform("Texture");
                        GLint projectionLocation = glShader->locationForUniform("Projection");
                        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                        auto transformComponent = gameObject->getComponent<TransformComponent>();
                        GLint modelLocation = glShader->locationForUniform("ModelView");
                        
                        if (transformComponent) {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transformComponent->worldMatrix_.m);
                        } else {
                            // TODO: This is a hack for now
                            Matrix4 mat;
                            
                            Matrix4MakeIdentify(mat);
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
                        }
                        
                        glDisable(GL_BLEND);
                        
                        glActiveTexture(GL_TEXTURE0);
                        setTexture(texture->getTarget(), texture->getHWTextureId());
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        
                        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), &vertices[0]);
                        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                        
                        glUniform1i(textureUniform, 0);
                        
                        glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                                       GL_UNSIGNED_BYTE, &Indices[0]);
#ifdef SUPPORT_PROFILING
                        ++numMasksDrawn_;
                        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
                    }
                }
            } else {
                NSLog(@"INDEED");
            }
        } else {
            NSLog(@"HWY");
        }
    } else {
        NSLog(@"HWY");
    }
}

void BGE::RenderServiceOpenGLES2::drawDebugQuads(std::vector<Vector3> points, Color &color) {
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Line"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint projectionLocation = glShader->locationForUniform("Projection");
    GLint modelLocation = glShader->locationForUniform("ModelView");
    GLint colorLocation = glShader->locationForUniform("Color");
    
    glEnableVertexAttribArray(positionLocation);
    glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);

    Matrix4 mat;
    
    Matrix4MakeIdentify(mat);
    glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
    
    glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);

    glLineWidth(2);

    for (auto index=0;index<points.size();index += 4) {
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), &points[index]);
        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) 4);
#ifdef SUPPORT_PROFILING
        ++numRectsDrawn_;
        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
    }
}

void BGE::RenderServiceOpenGLES2::drawLines(GameObject *gameObject) {
    if (gameObject) {
        auto line = gameObject->getComponent<LineRenderComponent>();
        
        if (line) {
            auto material = line->getMaterial();
            auto xform = gameObject->getComponent<TransformComponent>();
            const auto& points = line->getPoints();
            Vector3 vertices[points.size()];

            uint32_t index = 0;
            
            for (auto const &pt : points) {
                vertices[index].x = pt.x;
                vertices[index].y = pt.y;
                vertices[index].z = 0;
                
                index++;
            }
            
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("Line"));
            
            GLint positionLocation = glShader->locationForAttribute("Position");
            GLint projectionLocation = glShader->locationForUniform("Projection");
            GLint modelLocation = glShader->locationForUniform("ModelView");
            GLint colorLocation = glShader->locationForUniform("Color");
            
            glEnableVertexAttribArray(positionLocation);
            
            Color color;
            
            material->getColor(color);
            glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);

            if (xform) {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) xform->worldMatrix_.m);
            } else {
                // TODO: This is a hack for now
                Matrix4 mat;
                
                Matrix4MakeIdentify(mat);
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
            }
            
            glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);
            // 2
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), &vertices[0]);
            glLineWidth(line->getThickness());
            // 3
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

void BGE::RenderServiceOpenGLES2::drawPolyLines(GameObject *gameObject) {
    if (gameObject) {
        auto line = gameObject->getComponent<PolyLineRenderComponent>();
        
        if (line) {
            auto xform = gameObject->getComponent<TransformComponent>();
            const auto& points = line->getPoints();
            const auto& colors = line->getColors();
            
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("PolyLine"));
            
            GLint positionLocation = glShader->locationForAttribute("Position");
            GLint colorLocation = glShader->locationForAttribute("SourceColor");
            GLint projectionLocation = glShader->locationForUniform("Projection");
            GLint modelLocation = glShader->locationForUniform("ModelView");
            
            glEnableVertexAttribArray(positionLocation);
            
            glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
            
            if (xform) {
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) xform->worldMatrix_.m);
            } else {
                // TODO: This is a hack for now
                Matrix4 mat;
                
                Matrix4MakeIdentify(mat);
                glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
            }
            
            glEnable(GL_BLEND);

            // Convert our data to what VASEr needs
            auto numColors = colors.size();
            VASEr::Vec2 *vPoints = new VASEr::Vec2[points.size()];
            VASEr::Color *vColors = new VASEr::Color[points.size()];
            
            assert(points.size() == numColors || numColors == 1);
            
            for (auto i=0;i<points.size();++i) {
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

void BGE::RenderServiceOpenGLES2::drawSprite(GameObject *gameObject) {
    if (gameObject) {
        auto sprite = gameObject->getComponent<SpriteRenderComponent>();
        
        if (sprite) {
            VertexTex *const vertices = sprite->getVertices();
            auto material = sprite->getMaterial();
            if (material) {
                auto textureHandle = material->getTextureHandle();
                auto texture = Game::getInstance()->getTextureService()->getTexture(textureHandle);
                
                if (texture) {
#ifdef NOT_YET
                    NSLog(@"Rendering sprite %s", texture->getName().c_str());
#endif
                    
                    if (texture && texture->isValid()) {
#if 0
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("ColorMatrixTexture"));
#else
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(useShaderProgram("FullColorTexture"));
#endif
                        
                        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        GLint positionLocation = glShader->locationForAttribute("Position");
                        //            texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        glEnableVertexAttribArray(positionLocation);
                        glEnableVertexAttribArray(texCoordLocation);
                        
                        GLint textureUniform = glShader->locationForUniform("Texture");
                        GLint projectionLocation = glShader->locationForUniform("Projection");
                        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                        auto transformComponent = gameObject->getComponent<TransformComponent>();
                        GLint modelLocation = glShader->locationForUniform("ModelView");
                        GLint colorMatrixLocation = glShader->locationForUniform("ColorMatrix");
                        GLint colorMatOffsetLocation = glShader->locationForUniform("ColorMatOffset");
                        auto colorMultiplierLocation = glShader->locationForUniform("ColorMultiplier");
                        auto colorOffsetLocation = glShader->locationForUniform("ColorOffset");
                        
                        if (transformComponent) {
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) transformComponent->worldMatrix_.m);
                        } else {
                            // TODO: This is a hack for now
                            Matrix4 mat;
                            
                            Matrix4MakeIdentify(mat);
                            glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) mat.m);
                        }
                        
                        glUniformMatrix4fv(colorMatrixLocation, 1, 0, (GLfloat *) currentColorMatrix_.matrix.m);
                        glUniform4fv(colorMatOffsetLocation, 1, (GLfloat *) currentColorMatrix_.offset.v);

                        glUniform4fv(colorMultiplierLocation, 1, (GLfloat *) currentColorTransform_.multiplier.v);
                        glUniform4fv(colorOffsetLocation, 1, (GLfloat *) currentColorTransform_.offset.v);
                        
                        glDisable(GL_BLEND);
                        
                        glActiveTexture(GL_TEXTURE0);
                        setTexture(texture->getTarget(), texture->getHWTextureId());
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        
                        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), &vertices[0]);
                        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                        
                        glUniform1i(textureUniform, 0);
                        
                        glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                                       GL_UNSIGNED_BYTE, &Indices[0]);
#ifdef SUPPORT_PROFILING
                        ++numSpritesDrawn_;
                        ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
                    }
                }
            } else {
                NSLog(@"INDEED SPRITE");
            }
        } else {
            NSLog(@"HWY");
        }
    } else {
        NSLog(@"HWY");
    }
}

void BGE::RenderServiceOpenGLES2::drawString(TextComponent *text, Font *font, TransformComponent *transform, ColorMatrix& colorMatrix, ColorTransform& colorTransform, bool minimum) {
    if (text->isMultiline()) {
        auto multiText = text->getMultiText();
        auto yPos = text->getMultiTextY();

        if (text->isDropShadow()) {
            auto off = text->getDropShadowOffset();
            drawString(multiText, font, off.x, off.y, yPos, text->getBoundsWidth(), transform, (Color&) text->getDropShadowColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
        }

        drawString(multiText, font, 0, 0, yPos, text->getBoundsWidth(), transform, (Color&) text->getColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
    } else {
        if (text->isDropShadow()) {
            auto off = text->getDropShadowOffset();
            drawString(text->getText(), font, transform->getWorldMatrixRaw(), text->getBoundsWidth(), off.x, off.y, (Color&) text->getDropShadowColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
        }

        drawString(text->getText(), font, transform->getWorldMatrixRaw(), text->getBoundsWidth(), 0, 0, (Color&) text->getColor(), colorMatrix, colorTransform, text->getHorizontalAlignment(), text->getVerticalAlignment(), minimum);
    }
}

void BGE::RenderServiceOpenGLES2::drawString(std::vector<std::string> &strs, Font *font, float xOffset, float yOffset, std::vector<float> &yPos, float defWidth, TransformComponent *transform, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    const float *rawMatrix = transform->getWorldMatrixRaw();
    auto index = 0;

    for (auto &str : strs) {
        float yOff = yPos[index] + yOffset;

        drawString(str, font, rawMatrix, defWidth, xOffset, yOff, color, colorMatrix, colorTransform, horizAlignment, vertAlignment, minimum);

        index++;
    }
}

void BGE::RenderServiceOpenGLES2::drawString(std::string str, Font *font, const float *rawMatrix, float defWidth, float xOffset, float yOffset, Color &color, ColorMatrix& colorMatrix, ColorTransform& colorTransform, FontHorizontalAlignment horizAlignment, FontVerticalAlignment vertAlignment, bool minimum) {
    auto textureAtlas = Game::getInstance()->getTextureService()->getTextureAtlas(font->getTextureAtlasHandle());

    if (str.length() > 0 && textureAtlas && textureAtlas->isValid()) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        // TODO: Adjustment based on alignment to be done here
        float x = xOffset;
        float y = yOffset;
        std::shared_ptr<RenderServiceOpenGLES2> renderer = std::dynamic_pointer_cast<RenderServiceOpenGLES2>(Game::getInstance()->getRenderService());

        const char *chars = str.c_str();
        FontGlyph glyph;
        uint16_t code;
        size_t length = str.length();
        auto texHandle = textureAtlas->getTextureHandle();
        auto tex = Game::getInstance()->getTextureService()->getTexture(texHandle);

        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(renderer->useShaderProgram("FullColorFont"));
        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
        GLint positionLocation = glShader->locationForAttribute("Position");

        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(texCoordLocation);

        GLint textureUniform = glShader->locationForUniform("Texture");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        GLint modelLocation = glShader->locationForUniform("ModelView");
        GLint colorUniform = glShader->locationForUniform("SourceColor");
        GLint colorMatrixLocation = glShader->locationForUniform("ColorMatrix");
        GLint colorMatOffsetLocation = glShader->locationForUniform("ColorMatOffset");
        auto colorMultiplierLocation = glShader->locationForUniform("ColorMultiplier");
        auto colorOffsetLocation = glShader->locationForUniform("ColorOffset");

        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) renderer->getProjectionMatrix()->m);
        glUniformMatrix4fv(modelLocation, 1, 0, (GLfloat *) rawMatrix);

        glUniformMatrix4fv(colorMatrixLocation, 1, 0, (GLfloat *) colorMatrix.matrix.m);
        glUniform4fv(colorMatOffsetLocation, 1, (GLfloat *) colorMatrix.offset.v);

        glUniform4fv(colorMultiplierLocation, 1, (GLfloat *) colorTransform.multiplier.v);
        glUniform4fv(colorOffsetLocation, 1, (GLfloat *) colorTransform.offset.v);


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

        glDisable(GL_BLEND);

        glActiveTexture(GL_TEXTURE0);

        uint16_t prev = 0;

        setTexture(tex->getTarget(), tex->getHWTextureId());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUniform1i(textureUniform, 0);
        glUniform4f(colorUniform, color.r, color.g, color.b, color.a);

        for (int i=0;i<length;i++) {
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
                auto tex = Game::getInstance()->getTextureService()->getTexture(texHandle);

                if (tex) {
                    const Vector2 *xys = tex->getXYs();
                    const Vector2 *uvs = tex->getUVs();

                    vertices[0].position.x = x + xys[0].x;
                    vertices[0].position.y = y + xys[0].y;
                    vertices[0].position.z = 0;
                    vertices[0].tex.x = uvs[0].x;
                    vertices[0].tex.y = uvs[0].y;

                    vertices[1].position.x = x + xys[1].x;
                    vertices[1].position.y = y + xys[1].y;
                    vertices[1].position.z = 0;
                    vertices[1].tex.x = uvs[1].x;
                    vertices[1].tex.y = uvs[1].y;

                    vertices[2].position.x = x + xys[2].x;
                    vertices[2].position.y = y + xys[2].y;
                    vertices[2].position.z = 0;
                    vertices[2].tex.x = uvs[2].x;
                    vertices[2].tex.y = uvs[2].y;

                    vertices[3].position.x = x + xys[3].x;
                    vertices[3].position.y = y + xys[3].y;
                    vertices[3].position.z = 0;
                    vertices[3].tex.x = uvs[3].x;
                    vertices[3].tex.y = uvs[3].y;

                    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), &vertices[0]);
                    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                          sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));

                    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                                   GL_UNSIGNED_BYTE, &indices[0]);
#ifdef SUPPORT_PROFILING
                    ++numFontCharactersDrawn_;
                    ++numDrawCalls_;
#endif /* SUPPORT_PROFILING */
                }

                gridX += glyph->second.getAdvance();
                x = gridX;
                y = gridY;
            }

            prev = code;
        }
    }
}


uint8_t BGE::RenderServiceOpenGLES2::enableMask(GameObject *gameObject) {
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
                drawMaskRect(gameObject);
            } else if (gameObject->hasComponent<TextureMaskComponent>()) {
                drawTextureMask(gameObject);
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

void BGE::RenderServiceOpenGLES2::queueRender(double time) {
    lock();

    if (!isBackgrounded()) {
        [this->getRenderWindow()->getView() display];
    }
    
    unlock();
}

void BGE::RenderServiceOpenGLES2::render()
{
    handleServicesLock();
    lock();

    if (isBackgrounded()) {
        unlock();
        handleServicesUnlock();
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
        
        ColorMatrixMakeIdentify(currentColorMatrix_);
        colorMatrixStack_.clear();  // TODO: remove?
        
        ColorTransformMakeIdentity(currentColorTransform_);
        colorTransformStack_.clear();
        
        // Reset render states
        shaderProgramStack_.clear();
        
//        NSLog(@"%f %f", this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        
        // TODO: Content scale factor
//        glViewport(0, 0, this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        glViewport(0, 0, this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth() * this->getRenderWindow()->getContentScaleFactor(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight() * this->getRenderWindow()->getContentScaleFactor());
        
        std::vector<SpaceHandle> spaceHandles = Game::getInstance()->getSpaceService()->getSpaces();
        
        for (auto const &handle : spaceHandles) {
            auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
            
            if (space && space->isActive() && space->isVisible()) {
                std::vector<GameObject *> objects;
                
                space->getRootGameObjects(objects);
                
                for (auto const &obj : objects) {
                    if (obj) {
                        renderGameObject(obj, true);
                    }
                }
            }
        }
        
        if (Game::getInstance()->showCollisionRects()) {
            // Draw collision rects if needed
            Color color = Color{ 1, 1, 1, 1 };
            drawDebugQuads(boundingBoxPoints_, color);
            
            // First draw normal bounds
            color = Color{ 1, 1, 0, 1 };
            drawDebugQuads(scaledBoundingBoxPoints_, color);
        }
        
        [glContext->getContext() presentRenderbuffer:GL_RENDERBUFFER];
    }

#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
    frameRateCalculator_.nextFrame();
#endif /* SUPPORT_PROFILING */

    unlock();
    handleServicesUnlock();
}

int8_t BGE::RenderServiceOpenGLES2::renderGameObject(GameObject *gameObj, bool root, bool hasNextSibling) {
    uint8_t maskValue = 0;

#ifdef SUPPORT_PROFILING
    ++numGameObjectsIgnored_;
    ++numProcessedObjects_;
#endif /* SUPPORT_PROFILING */

    if (!gameObj->isActive()) {
        return maskValue;
    }
    
    // TODO: Transform
    auto transformComponent = gameObj->getComponent<TransformComponent>();
    
    if (transformComponent) {
        if (!transformComponent->canRender()) {
            return maskValue;
        }

        auto parent = transformComponent->getParent();

        if (root && parent) {
            return maskValue;
        } else if (!root && !parent) {
            return maskValue;
        }

        // Need to update matrix
        transformComponent->updateMatrix();

        // Since we have the transform, push our
        pushColorMatrix();
        pushColorTransform();
        
        auto colorMatrix = gameObj->getComponent<ColorMatrixComponent>();
        auto colorTransform = gameObj->getComponent<ColorTransformComponent>();
        
        if (colorMatrix) {
            auto tMat = currentColorMatrix_ * colorMatrix->matrix;
            currentColorMatrix_ = tMat;
        }
        
        if (colorTransform) {
            auto tColor = currentColorTransform_ * colorTransform->transform;
            currentColorTransform_ = tColor;
        }
        
        if (gameObj->hasComponent<SpriteRenderComponent>()) {
            drawSprite(gameObj);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<TextComponent>()) {
            auto text = gameObj->getComponent<TextComponent>();
            Font *font = Game::getInstance()->getFontService()->getFont(text->getFontHandle());
            
            if (font) {
                drawString(text, font, transformComponent, currentColorMatrix_, currentColorTransform_);
            }
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<PolyLineRenderComponent>()) {
            drawPolyLines(gameObj);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<LineRenderComponent>()) {
            drawLines(gameObj);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<FlatRectRenderComponent>()) {
            drawFlatRect(gameObj);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        } else if (gameObj->hasComponent<MaskComponent>() || gameObj->hasComponent<TextureMaskComponent>()) {
            maskValue = enableMask(gameObj);
#ifdef SUPPORT_PROFILING
            ++numGameObjectsDrawn_;
            --numGameObjectsIgnored_;   // We tagged ourselves as ignored, remove us from the count as we are actually drawn
#endif /* SUPPORT_PROFILING */
        }

        uint8_t childrenMasks = 0;;
        
        // Determine if we have children, if we do process them.
        auto childrenHandles = transformComponent->getOrderedChildrenHandles();
        for (auto i=0;i<childrenHandles.size();++i) {
            auto childXformHandle = childrenHandles[i];
            auto childXform = componentService_->getComponent<TransformComponent>(childXformHandle.getHandle());
            
            if (childXform->hasGameObject()) {
                auto childObjHandle = childXform->getGameObjectHandle();
                auto childObj = childXform->getSpace()->getGameObject(childObjHandle);
                
                if (childObj) {
                    childrenMasks |= renderGameObject(childObj, false, i < (transformComponent->getNumChildren() - 1));
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

void BGE::RenderServiceOpenGLES2::setTexture(GLenum target, GLuint texId) {
    if (currentTextureId_ != texId) {
        glBindTexture(target, texId);
        currentTextureId_ = texId;
#ifdef SUPPORT_PROFILING
        ++numTexturesChanged_;
#endif /* SUPPORT_PROFILING */
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
    glDeleteTextures(1, &data->glHwId);
    if (item.callback) {
        item.callback(item, std::make_shared<Error>(RenderService::ErrorDomain, static_cast<int32_t>(RenderServiceError::Unimplemented)));
    }
}

#ifdef SUPPORT_PROFILING

void BGE::RenderServiceOpenGLES2::resetProfilingStats() {
    RenderService::resetProfilingStats();
}

#endif /* SUPPORT_PROFILING */

