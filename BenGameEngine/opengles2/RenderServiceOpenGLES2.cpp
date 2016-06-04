//
//  RenderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderServiceOpenGLES2.h"
#include "RenderView.h"
#include "FontServiceOpenGLES2.h"
#include "ShaderServiceOpenGLES2.h"
#include "RenderContextOpenGLES2.h"
#include "TextureOpenGLES2.h"
#include "MathTypes.h"
#include "Game.h"
#include "LineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"

#if 0
const BGE::VertexColor Vertices[] = {
    {{100, -100, 0}, {1, 0, 0, 1}},
    {{100, 100, 0}, {0, 1, 0, 1}},
    {{-100, 100, 0}, {0, 0, 1, 1}},
    {{-100, -100, 0}, {0, 0, 0, 1}}
};
#else
const BGE::VertexColor Vertices[] = {
    {{620, 0, 0}, {1, 0, 0, 1}},
    {{620, 200, 0}, {0, 1, 0, 1}},
    {{0, 200, 0}, {0, 0, 1, 1}},
    {{0, 0, 0}, {0, 0, 0, 1}}
};
#endif

const BGE::VertexColorTex TextureVertices[] = {
    {{750, 100, 0}, {1, 0, 0, 1}, { 1, 0 } },
    {{750, 200, 0}, {0, 1, 0, 1}, { 1, 1 } },
    {{50, 200, 0}, {0, 0, 1, 1}, { 0, 1 } },
    {{50, 100, 0}, {0, 0, 0, 1}, { 0, 0 } }
};

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

BGE::RenderServiceOpenGLES2::RenderServiceOpenGLES2() : masksInUse_(0), activeMasks_(0) {
    shaderService_ = std::make_shared<ShaderServiceOpenGLES2>();
    ShaderServiceOpenGLES2::mapShaderBundle("BenGameEngineBundle");
    FontServiceOpenGLES2::mapBundles("BenGameEngineBundle");
    
    Matrix4MakeIdentify(projectionMatrix_);
    
    Game::getInstance()->getHeartbeatService()->registerListener("Renderer", std::bind(&RenderServiceOpenGLES2::queueRender, this), 0);
}

void BGE::RenderServiceOpenGLES2::initialize() {}
void BGE::RenderServiceOpenGLES2::reset() {}
void BGE::RenderServiceOpenGLES2::enteringBackground() {}
void BGE::RenderServiceOpenGLES2::enteringForeground() {}
void BGE::RenderServiceOpenGLES2::pause() {}
void BGE::RenderServiceOpenGLES2::resume() {}
void BGE::RenderServiceOpenGLES2::destroy() {}

void BGE::RenderServiceOpenGLES2::setCoordinateSystem2D(Render2DCoordinateSystem coordSystem2D)
{
    BGE::RenderService::setCoordinateSystem2D(coordSystem2D);
    
    std::shared_ptr<BGE::RenderWindow> window = this->getRenderWindow();
    
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

void BGE::RenderServiceOpenGLES2::bindRenderWindow(std::shared_ptr<BGE::RenderContext> context, std::shared_ptr<BGE::RenderWindow> window)
{
    RenderService::bindRenderWindow(context, window);
    std::shared_ptr<BGE::RenderContextOpenGLES2> glContext;
    
    glContext = std::dynamic_pointer_cast<BGE::RenderContextOpenGLES2>(context);
    
    if (glContext) {
        window->getView().context = glContext->getContext();
        window->getView().drawableStencilFormat = GLKViewDrawableStencilFormat8;
    }
    
    // Create the ortho matrix
//    Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), window->getHeight(), 0, -1, 1);
//    Matrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), 0, window->getHeight(), -1, 1);
//    Matrix4Scale(projectionMatrix_, 1, -1, 1);
//    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)window->getView().layer;
//    eaglLayer.contentsScale = window->getView().contentScaleFactor;

#if 0
    setCoordinateSystem2D(Render2DCoordinateSystem::OpenGL);
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
    program = this->getShaderService()->createShaderProgram("ColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "colorMatrix", "colorOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "MaskColorMatrixTextureVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "MaskColorMatrixTextureFragment");
    program = this->getShaderService()->createShaderProgram("MaskColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn", "MaskTexCoordIn" }, { "ModelView", "Projection", "Texture", "MaskTexture", "colorMatrix", "colorOffset" });
    
    vShader = this->getShaderService()->createShader(ShaderType::Vertex, "FontVertex");
    fShader = this->getShaderService()->createShader(ShaderType::Fragment, "FontFragment");
    program = this->getShaderService()->createShaderProgram("Font", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "SourceColor" });
}

std::shared_ptr<BGE::ShaderProgram> BGE::RenderServiceOpenGLES2::pushShaderProgram(std::string program)
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
        
        for (auto &kv: glProgram->getAttributes()) {
            
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
    
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
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
    
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
    
#if 1
    if (textureInfo_) {
        glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("ColorMatrixTexture"));
        
        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
        GLint colorMatrixLocation = glShader->locationForUniform("colorMatrix");
        GLint colorOffsetLocation = glShader->locationForUniform("colorOffset");
        
        positionLocation = glShader->locationForAttribute("Position");
        colorLocation = glShader->locationForAttribute("SourceColor");
        texCoordLocation = glShader->locationForAttribute("TexCoordIn");
        
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(colorLocation);
        glEnableVertexAttribArray(texCoordLocation);

        GLint textureUniform = glShader->locationForUniform("Texture");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);

        Matrix4 colorMatrix;
        Vector4 colorOffset;
        
        Matrix4MakeIdentify(colorMatrix);
        
#if 1
        colorMatrix.m[0] = -35.48653030395508 / 255.0;
        colorMatrix.m[1] = 22.864973068237305 / 255.0;
        colorMatrix.m[2] = 23.621559143066406 / 255.0;

        colorMatrix.m[4] = 465.0001220703125 / 255.0;
        colorMatrix.m[5] = 10.450590133666992 / 255.0;
        colorMatrix.m[6] = 3.5698392391204834 / 255.0;
        colorMatrix.m[7] = -3.0204293727874756 / 255.0;
        
        colorMatrix.m[8] = 0;
        colorMatrix.m[9] = 465.0000305175781 / 255.0;
        colorMatrix.m[10] = -10.0094032287597669 / 255.0;
        colorMatrix.m[11] = 54.32495880126953 / 255.0;
        
        colorMatrix.m[12] = -33.3155517578125 / 255.0;
        colorMatrix.m[13] = 0;
        colorMatrix.m[14] = 465 / 255.0;
#else
        colorMatrix.m[0] = -35.48653030395508 / 255.0;
        colorMatrix.m[4] = 22.864973068237305 / 255.0;
        colorMatrix.m[8] = 23.621559143066406 / 255.0;
        colorMatrix.m[12] = 0;

        colorMatrix.m[1] = 465.0001220703125 / 255.0;
        colorMatrix.m[5] = 10.450590133666992 / 255.0;
        colorMatrix.m[9] = 3.5698392391204834 / 255.0;
        colorMatrix.m[13] = -3.0204293727874756 / 255.0;
        
        colorMatrix.m[2] = 0;
        colorMatrix.m[6] = 465.0000305175781 / 255.0;
        colorMatrix.m[10] = -10.0094032287597669 / 255.0;
        colorMatrix.m[14] = 54.32495880126953 / 255.0;
        
        colorMatrix.m[3] = -33.3155517578125 / 255.0;
        colorMatrix.m[7] = 0;
        colorMatrix.m[11] = 465 / 255.0;
        colorMatrix.m[15] = 0;
#endif
        glUniformMatrix4fv(colorMatrixLocation, 1, 0, (GLfloat *) colorMatrix.m);
        
        colorOffset.r = 0;
        colorOffset.g = 1;
        colorOffset.b = 1;
        colorOffset.a = 0;
        
        glUniform4fv(colorOffsetLocation, 1, (GLfloat *) colorOffset.v);
        
        glDisable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//        glEnable(GL_BLEND);
//        glBlendEquation( GL_FUNC_ADD );
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(textureInfo_.target, textureInfo_.name);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                              sizeof(VertexColorTex), &TextureVertices[0]);
        glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(VertexColorTex), (GLvoid*) (&TextureVertices[0].color));
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                              sizeof(VertexColorTex), (GLvoid*) (&TextureVertices[0].tex));
        
        glUniform1i(textureUniform, 0);
        
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                       GL_UNSIGNED_BYTE, &indices[0]);
        
	    }
#endif
}

void BGE::RenderServiceOpenGLES2::drawFont(Vector2 &position, std::shared_ptr<TextureBase> texture) {
    if (texture) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        std::shared_ptr<TextureOpenGLES2> oglTex = std::dynamic_pointer_cast<TextureOpenGLES2>(texture);
        
        if (oglTex && oglTex->isValid()) {
            const Vector2 *xys = oglTex->getXYs();
            const Vector2 *uvs = oglTex->getUVs();
            
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
            
            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Font"));
            
            GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
            GLint positionLocation = glShader->locationForAttribute("Position");
            
            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(texCoordLocation);
            
            GLint textureUniform = glShader->locationForUniform("Texture");
            GLint projectionLocation = glShader->locationForUniform("Projection");
            GLint colorUniform = glShader->locationForUniform("SourceColor");
            
            glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
            
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
            
            glUniform1i(textureUniform, 0);
            glUniform4f(colorUniform, 1, 0, 0, 1);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                           GL_UNSIGNED_BYTE, &indices[0]);
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawTexture(Vector2 &position, std::shared_ptr<TextureBase> texture)
{
    if (texture) {
        VertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        std::shared_ptr<TextureOpenGLES2> oglTex = std::dynamic_pointer_cast<TextureOpenGLES2>(texture);

        if (oglTex && oglTex->isValid()) {
            const Vector2 *xys = oglTex->getXYs();
            const Vector2 *uvs = oglTex->getUVs();
            
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

            std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Texture"));
            
            GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
            
            GLint positionLocation = glShader->locationForAttribute("Position");
//            texCoordLocation = glShader->locationForAttribute("TexCoordIn");
            
            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(texCoordLocation);
            
            GLint textureUniform = glShader->locationForUniform("Texture");
            GLint projectionLocation = glShader->locationForUniform("Projection");
            glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
            
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
            
            glUniform1i(textureUniform, 0);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                           GL_UNSIGNED_BYTE, &indices[0]);
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawFlatRect(std::shared_ptr<BGE::GameObject> gameObject) {
    if (gameObject) {
        std::shared_ptr<BGE::FlatRectRenderComponent> flatRect = std::dynamic_pointer_cast<BGE::FlatRectRenderComponent>(gameObject->getComponent<BGE::FlatRectRenderComponent>());
        
        if (flatRect) {
            Vertex *const vertices = flatRect->getVertices();
            std::shared_ptr<BGE::Material> material = flatRect->getMaterial().lock();
            
            if (material) {
                std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Line"));

                GLint positionLocation = glShader->locationForAttribute("Position");
                GLint projectionLocation = glShader->locationForUniform("Projection");
                GLint modelLocation = glShader->locationForUniform("ModelView");
                GLint colorLocation = glShader->locationForUniform("Color");
                
                glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                      sizeof(Vertex), &vertices[0]);
                
                Color color;
                
                material->getColor(color);
                glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);
                glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                               GL_UNSIGNED_BYTE, &Indices[0]);

            }
        }
    }
}

void BGE::RenderServiceOpenGLES2::drawLines(const std::vector<Vector2>& points, float thickness, bool loop, std::shared_ptr<BGE::Material> material) {
    Vector3 vertices[points.size()];
    GLubyte indices[points.size()];
    
    uint32_t index = 0;
    
    for (auto pt : points) {
        vertices[index].x = pt.x;
        vertices[index].y = pt.y;
        vertices[index].z = 0;
        
        indices[index] = index;
        
        index++;
    }
    std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Line"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint projectionLocation = glShader->locationForUniform("Projection");
    GLint modelLocation = glShader->locationForUniform("ModelView");
    GLint colorLocation = glShader->locationForUniform("Color");
    
    glEnableVertexAttribArray(positionLocation);
    
    Color color;
    
    material->getColor(color);
    glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
    glUniform4fv(colorLocation, 1, (GLfloat *) &color.v[0]);
    // 2
    glVertexAttribPointer(positionLocation, points.size(), GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), &vertices[0]);
    glLineWidth(12);
    // 3
    glDrawArrays(GL_LINE_LOOP, 0, points.size());
}


void BGE::RenderServiceOpenGLES2::drawSprite(std::shared_ptr<BGE::GameObject> gameObject) {
    if (gameObject) {
        std::shared_ptr<BGE::SpriteRenderComponent> sprite = std::dynamic_pointer_cast<BGE::SpriteRenderComponent>(gameObject->getComponent<BGE::SpriteRenderComponent>());
        
        if (sprite) {
            VertexTex *const vertices = sprite->getVertices();
            std::shared_ptr<BGE::Material> material = sprite->getMaterial().lock();
            if (material) {
                std::shared_ptr<TextureBase> texture = material->getTexture().lock();
                
                if (texture) {
                    std::shared_ptr<TextureOpenGLES2> oglTex = std::dynamic_pointer_cast<TextureOpenGLES2>(texture);
                    if (oglTex && oglTex->isValid()) {
                        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Texture"));
                        
                        GLint texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        GLint positionLocation = glShader->locationForAttribute("Position");
                        //            texCoordLocation = glShader->locationForAttribute("TexCoordIn");
                        
                        glEnableVertexAttribArray(positionLocation);
                        glEnableVertexAttribArray(texCoordLocation);
                        
                        GLint textureUniform = glShader->locationForUniform("Texture");
                        GLint projectionLocation = glShader->locationForUniform("Projection");
                        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
                        
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_BLEND);
                        
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(oglTex->getTarget(), oglTex->getHWTextureId());
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        
                        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), &vertices[0]);
                        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                              sizeof(VertexTex), (GLvoid*) (&vertices[0].tex));
                        
                        glUniform1i(textureUniform, 0);
                        
                        glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                                       GL_UNSIGNED_BYTE, &Indices[0]);
                    }
                }
            }
        }
    }
}

int8_t BGE::RenderServiceOpenGLES2::createMask(Vector2 &position, std::shared_ptr<TextureBase> mask)
{
    if (this->masksInUse_ < (RenderServiceOpenGLES2::MaxActiveMasks - 1)) {
        int8_t maskId = this->masksInUse_;
        uint8_t maskValue = MaskIdToMaskValue[this->masksInUse_];
        this->masksInUse_++;
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 2, ~0);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
//        glStencilMask(0xFF);
        
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        
        drawTexture(position, mask);
        
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//        glStencilMask(0);
        glStencilFunc(GL_NOTEQUAL, 2, ~0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        return maskId;
    } else {
        return -1;
    }
}

void BGE::RenderServiceOpenGLES2::enableMask(int8_t maskId)
{
    if (maskId >=0 && maskId < this->masksInUse_) {
        
    }
}

void BGE::RenderServiceOpenGLES2::disableMask(int8_t maskId)
{
    if (maskId >=0 && maskId < this->masksInUse_) {
        
    }
}

void BGE::RenderServiceOpenGLES2::queueRender() {
    [this->getRenderWindow()->getView() display];
}

void BGE::RenderServiceOpenGLES2::render()
{
    std::shared_ptr<BGE::RenderContextOpenGLES2> glContext = std::dynamic_pointer_cast<BGE::RenderContextOpenGLES2>(getRenderContext());
    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    this->masksInUse_ = 0;
    
    if (isReady()) {
        std::shared_ptr<TextureBase> texture = Game::getInstance()->getTextureService()->textureWithName("sample");
        std::shared_ptr<TextureBase> fish = Game::getInstance()->getTextureService()->textureWithName("CoinIcon");
        std::shared_ptr<TextureBase> font = Game::getInstance()->getTextureService()->textureWithName("__font_texture");
        std::shared_ptr<Font> f = Game::getInstance()->getFontService()->getFont("default", 32);
        
        if (f) {
            NSLog(@"STRING LENGTH %d", f->getStringWidth("HELLO", false));
        }
        // Reset render states
        shaderProgramStack_.clear();
        
//        NSLog(@"%f %f", this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        
        // TODO: Content scale factor
//        glViewport(0, 0, this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight());
        glViewport(0, 0, this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getWidth() * this->getRenderWindow()->getContentScaleFactor(), this->getRenderWindow()->getRenderView(RenderWindow::DefaultRenderViewName)->getHeight() * this->getRenderWindow()->getContentScaleFactor());
        
        std::shared_ptr<ShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<ShaderProgramOpenGLES2>(pushShaderProgram("Default"));
        
        GLint positionLocation = glShader->locationForAttribute("Position");
        GLint colorLocation = glShader->locationForAttribute("SourceColor");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        GLint modelLocation = glShader->locationForUniform("ModelView");
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(colorLocation);
        
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
        
        // 2
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                              sizeof(VertexColor), &Vertices[0]);
        glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(VertexColor), (GLvoid*) (&Vertices[0].color));
        
        // 3
        glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                       GL_UNSIGNED_BYTE, &Indices[0]);
        
        Vector2 position = { 0, 0 };
        Vector2 size = { 150, 300 };
        Vector4 colors[4] = {{ 1, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 1, 0, 1 }, { 0, 0, 1, 1 }};
        
        drawShadedRect(position, size, colors);
        if (font) {
            position = { 400, 400 };
            drawFont(position, font);
        }
        
        
        if (textureInfo_) {
            
        }
        
        if (texture && fish) {
            
            position = { 200, 400 };
            createMask(position, texture);
            drawTexture(position, fish);
            glDisable(GL_STENCIL_TEST);
            NSLog(@"HERE");
            
            if (f) {
                position = { 200, 1200 };
                f->drawString("HELLO YOU SACK OF DIRT AV abcDefg1210312084pdsoinjm_-'\"#*$&%", position, colors[1]);
            }
        }
        
        
        for (auto obj : Game::getInstance()->getGameObjectService()->getGameObjects()) {
            if (obj.second->getComponent<BGE::LineRenderComponent>()) {
                std::shared_ptr<BGE::LineRenderComponent> line = obj.second->getComponent<BGE::LineRenderComponent>();
                
                drawLines(line->getPoints(), line->getThickness(), line->isLineLoop(), line->getMaterial().lock());
            } else if (obj.second->getComponent<BGE::FlatRectRenderComponent>()) {
                drawFlatRect(obj.second);
            } else if (obj.second->getComponent<BGE::SpriteRenderComponent>()) {
                drawSprite(obj.second);
            }
        }
        
        [glContext->getContext() presentRenderbuffer:GL_RENDERBUFFER];
    }
}