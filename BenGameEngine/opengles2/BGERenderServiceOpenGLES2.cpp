//
//  BGERenderServiceOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGERenderServiceOpenGLES2.h"
#include "BGERenderView.h"
#include "BGEFontServiceOpenGLES2.h"
#include "BGEShaderServiceOpenGLES2.h"
#include "BGERenderContextOpenGLES2.h"
#include "BGETextureOpenGLES2.h"
#include "BGEMathTypes.h"
#include "BGEGame.h"

#if 0
const BGEVertexColor Vertices[] = {
    {{100, -100, 0}, {1, 0, 0, 1}},
    {{100, 100, 0}, {0, 1, 0, 1}},
    {{-100, 100, 0}, {0, 0, 1, 1}},
    {{-100, -100, 0}, {0, 0, 0, 1}}
};
#else
const BGEVertexColor Vertices[] = {
    {{620, 0, 0}, {1, 0, 0, 1}},
    {{620, 200, 0}, {0, 1, 0, 1}},
    {{0, 200, 0}, {0, 0, 1, 1}},
    {{0, 0, 0}, {0, 0, 0, 1}}
};
#endif

const BGEVertexColorTex TextureVertices[] = {
    {{750, 100, 0}, {1, 0, 0, 1}, { 1, 0 } },
    {{750, 200, 0}, {0, 1, 0, 1}, { 1, 1 } },
    {{50, 200, 0}, {0, 0, 1, 1}, { 0, 1 } },
    {{50, 100, 0}, {0, 0, 0, 1}, { 0, 0 } }
};

const GLubyte Indices[] = {
    0, 1, 2,
    2, 3, 0
};

int8_t BGERenderServiceOpenGLES2::MaxActiveMasks = 8;
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

BGERenderServiceOpenGLES2::BGERenderServiceOpenGLES2() : masksInUse_(0), activeMasks_(0) {
    shaderService_ = std::make_shared<BGEShaderServiceOpenGLES2>();
    BGEShaderServiceOpenGLES2::mapShaderBundle("BenGameEngineBundle");
    BGEFontServiceOpenGLES2::mapBundles("BenGameEngineBundle");
    
    BGEMatrix4MakeIdentify(projectionMatrix_);
    
    BGEGame::getInstance()->getHeartbeatService()->registerListener("Renderer", std::bind(&BGERenderServiceOpenGLES2::queueRender, this), 0);
}

void BGERenderServiceOpenGLES2::initialize() {}
void BGERenderServiceOpenGLES2::reset() {}
void BGERenderServiceOpenGLES2::enteringBackground() {}
void BGERenderServiceOpenGLES2::enteringForeground() {}
void BGERenderServiceOpenGLES2::pause() {}
void BGERenderServiceOpenGLES2::resume() {}
void BGERenderServiceOpenGLES2::destroy() {}

void BGERenderServiceOpenGLES2::setCoordinateSystem2D(BGERender2DCoordinateSystem coordSystem2D)
{
    BGERenderService::setCoordinateSystem2D(coordSystem2D);
    
    std::shared_ptr<BGERenderWindow> window = this->getRenderWindow();
    
    switch (coordSystem2D) {
        case BGERender2DCoordinateSystem::Traditional:
            BGEMatrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor(), window->getHeight() * this->getRenderWindow()->getContentScaleFactor(), 0, -1, 1);
            break;
        case BGERender2DCoordinateSystem::TraditionalCentered:
            BGEMatrix4MakeOrthographic(projectionMatrix_, -window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -1, 1);
            break;
        case BGERender2DCoordinateSystem::OpenGL:
            BGEMatrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor(), 0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor(), -1, 1);
            break;
        case BGERender2DCoordinateSystem::OpenGLCentered:
            BGEMatrix4MakeOrthographic(projectionMatrix_, -window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getWidth() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, window->getHeight() * this->getRenderWindow()->getContentScaleFactor() / 2.0, -1, 1);
            break;
    }
}

void BGERenderServiceOpenGLES2::bindRenderWindow(std::shared_ptr<BGERenderContext> context, std::shared_ptr<BGERenderWindow> window)
{
    BGERenderService::bindRenderWindow(context, window);
    std::shared_ptr<BGERenderContextOpenGLES2> glContext;
    
    glContext = std::dynamic_pointer_cast<BGERenderContextOpenGLES2>(context);
    
    if (glContext) {
        window->getView().context = glContext->getContext();
        window->getView().drawableStencilFormat = GLKViewDrawableStencilFormat8;
    }
    
    // Create the ortho matrix
//    BGEMatrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), window->getHeight(), 0, -1, 1);
//    BGEMatrix4MakeOrthographic(projectionMatrix_, 0, window->getWidth(), 0, window->getHeight(), -1, 1);
//    BGEMatrix4Scale(projectionMatrix_, 1, -1, 1);
//    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)window->getView().layer;
//    eaglLayer.contentsScale = window->getView().contentScaleFactor;

#if 0
    setCoordinateSystem2D(BGERender2DCoordinateSystem::OpenGL);
#else
    setCoordinateSystem2D(BGERender2DCoordinateSystem::Traditional);
#endif
}

void BGERenderServiceOpenGLES2::resizeRenderWindow()
{
}

void BGERenderServiceOpenGLES2::createShaders()
{
    std::shared_ptr<BGEShader> vShader = this->getShaderService()->createShader(BGEShaderType::Vertex, "SimpleVertex");
    std::shared_ptr<BGEShader> fShader = this->getShaderService()->createShader(BGEShaderType::Fragment, "SimpleFragment");
    std::shared_ptr<BGEShaderProgram> program = this->getShaderService()->createShaderProgram("Default", {vShader,  fShader}, { "Position", "SourceColor" }, { "ModelView", "Projection" });
    
    vShader = this->getShaderService()->createShader(BGEShaderType::Vertex, "TextureVertex");
    fShader = this->getShaderService()->createShader(BGEShaderType::Fragment, "TextureFragment");
    program = this->getShaderService()->createShaderProgram("Texture", {vShader,  fShader}, { "Position", "SourceColor", "TexCoordIn" }, { "ModelView", "Projection", "Texture" });
    
    vShader = this->getShaderService()->createShader(BGEShaderType::Vertex, "ColorMatrixTextureVertex");
    fShader = this->getShaderService()->createShader(BGEShaderType::Fragment, "ColorMatrixTextureFragment");
    program = this->getShaderService()->createShaderProgram("ColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "colorMatrix", "colorOffset" });
    
    vShader = this->getShaderService()->createShader(BGEShaderType::Vertex, "MaskColorMatrixTextureVertex");
    fShader = this->getShaderService()->createShader(BGEShaderType::Fragment, "MaskColorMatrixTextureFragment");
    program = this->getShaderService()->createShaderProgram("MaskColorMatrixTexture", {vShader,  fShader}, { "Position", "TexCoordIn", "MaskTexCoordIn" }, { "ModelView", "Projection", "Texture", "MaskTexture", "colorMatrix", "colorOffset" });
    
    vShader = this->getShaderService()->createShader(BGEShaderType::Vertex, "FontVertex");
    fShader = this->getShaderService()->createShader(BGEShaderType::Fragment, "FontFragment");
    program = this->getShaderService()->createShaderProgram("Font", {vShader,  fShader}, { "Position", "TexCoordIn" }, { "ModelView", "Projection", "Texture", "SourceColor" });
}

std::shared_ptr<BGEShaderProgram> BGERenderServiceOpenGLES2::pushShaderProgram(std::string program)
{
    std::string currShader;
    
    if (!shaderProgramStack_.empty()) {
        currShader = shaderProgramStack_.back()->getName();
    }
    
    std::shared_ptr<BGEShaderProgram> shaderProgram = shaderService_->getShaderProgram(program);
    
    // TODO: Assert if shader does not exist
    shaderProgramStack_.push_back(shaderProgram);
    
    if (currShader != program) {
        std::shared_ptr<BGEShaderProgramOpenGLES2> glProgram = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(shaderProgramStack_.back());
        
        glUseProgram(glProgram->getProgram());
        
        for (auto &kv: glProgram->getAttributes()) {
            
        }
    }
    
    return shaderProgramStack_.back();
}

std::shared_ptr<BGEShaderProgram> BGERenderServiceOpenGLES2::popShaderProgram()
{
    if (!shaderProgramStack_.empty()) {
        std::string currShader = shaderProgramStack_.back()->getName();
        
        shaderProgramStack_.pop_back();
        
        if (!shaderProgramStack_.empty()) {
            std::shared_ptr<BGEShaderProgramOpenGLES2> program = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(shaderProgramStack_.back());
            
            if (currShader != program->getName()) {
                // We have a different shader
                glUseProgram(program->getProgram());
            }
        }
    }
    
    return shaderProgramStack_.back();
}

void BGERenderServiceOpenGLES2::drawRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 &color)
{
    BGEVertexColor vertices[4];
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
    
    std::shared_ptr<BGEShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(BGEVertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(BGEVertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
}

void BGERenderServiceOpenGLES2::drawShadedRect(BGEVector2 &position, BGEVector2 &size, BGEVector4 color[4])
{
    BGEVertexColor vertices[4];
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
    
    std::shared_ptr<BGEShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("Default"));
    
    GLint positionLocation = glShader->locationForAttribute("Position");
    GLint colorLocation = glShader->locationForAttribute("SourceColor");
    
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(BGEVertexColor), &vertices[0]);
    glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(BGEVertexColor), (GLvoid*) (&vertices[0].color));
    
    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                   GL_UNSIGNED_BYTE, &indices[0]);
    
#if 1
    if (textureInfo_) {
        glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("ColorMatrixTexture"));
        
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

        BGEMatrix4 colorMatrix;
        BGEVector4 colorOffset;
        
        BGEMatrix4MakeIdentify(colorMatrix);
        
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
                              sizeof(BGEVertexColorTex), &TextureVertices[0]);
        glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(BGEVertexColorTex), (GLvoid*) (&TextureVertices[0].color));
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                              sizeof(BGEVertexColorTex), (GLvoid*) (&TextureVertices[0].tex));
        
        glUniform1i(textureUniform, 0);
        
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                       GL_UNSIGNED_BYTE, &indices[0]);
        
	    }
#endif
}

void BGERenderServiceOpenGLES2::drawFont(BGEVector2 &position, std::shared_ptr<BGETextureBase> texture) {
    if (texture) {
        BGEVertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        std::shared_ptr<BGETextureOpenGLES2> oglTex = std::dynamic_pointer_cast<BGETextureOpenGLES2>(texture);
        
        if (oglTex && oglTex->isValid()) {
            const BGEVector2 *xys = oglTex->getXYs();
            const BGEVector2 *uvs = oglTex->getUVs();
            
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
            
            std::shared_ptr<BGEShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("Font"));
            
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
                                  sizeof(BGEVertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(BGEVertexTex), (GLvoid*) (&vertices[0].tex));
            
            glUniform1i(textureUniform, 0);
            glUniform4f(colorUniform, 1, 0, 0, 1);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                           GL_UNSIGNED_BYTE, &indices[0]);
        }
    }
}

void BGERenderServiceOpenGLES2::drawTexture(BGEVector2 &position, std::shared_ptr<BGETextureBase> texture)
{
    if (texture) {
        BGEVertexTex vertices[4];
        GLubyte indices[6] = { 0, 1, 2, 0, 2, 3 };  // TODO: Make these indices constant
        std::shared_ptr<BGETextureOpenGLES2> oglTex = std::dynamic_pointer_cast<BGETextureOpenGLES2>(texture);

        if (oglTex && oglTex->isValid()) {
            const BGEVector2 *xys = oglTex->getXYs();
            const BGEVector2 *uvs = oglTex->getUVs();
            
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

            std::shared_ptr<BGEShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("Texture"));
            
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
                                  sizeof(BGEVertexTex), &vertices[0]);
            glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(BGEVertexTex), (GLvoid*) (&vertices[0].tex));
            
            glUniform1i(textureUniform, 0);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]),
                           GL_UNSIGNED_BYTE, &indices[0]);
        }
    }
}

int8_t BGERenderServiceOpenGLES2::createMask(BGEVector2 &position, std::shared_ptr<BGETextureBase> mask)
{
    if (this->masksInUse_ < (BGERenderServiceOpenGLES2::MaxActiveMasks - 1)) {
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

void BGERenderServiceOpenGLES2::enableMask(int8_t maskId)
{
    if (maskId >=0 && maskId < this->masksInUse_) {
        
    }
}

void BGERenderServiceOpenGLES2::disableMask(int8_t maskId)
{
    if (maskId >=0 && maskId < this->masksInUse_) {
        
    }
}

void BGERenderServiceOpenGLES2::queueRender() {
    [this->getRenderWindow()->getView() display];
}

void BGERenderServiceOpenGLES2::render()
{
    std::shared_ptr<BGERenderContextOpenGLES2> glContext = std::dynamic_pointer_cast<BGERenderContextOpenGLES2>(getRenderContext());
    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    this->masksInUse_ = 0;
    
    if (isReady()) {
        std::shared_ptr<BGETextureBase> texture = BGEGame::getInstance()->getTextureService()->textureWithName("sample");
        std::shared_ptr<BGETextureBase> fish = BGEGame::getInstance()->getTextureService()->textureWithName("fish");
        std::shared_ptr<BGETextureBase> font = BGEGame::getInstance()->getTextureService()->textureWithName("__font_texture");
        std::shared_ptr<BGEFont> f = BGEGame::getInstance()->getFontService()->getFont("default", 32);
        
        if (f) {
            NSLog(@"STRING LENGTH %d", f->getStringWidth("HELLO", false));
        }
        // Reset render states
        shaderProgramStack_.clear();
        
//        NSLog(@"%f %f", this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getHeight());
        
        // TODO: Content scale factor
//        glViewport(0, 0, this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getWidth(), this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getHeight());
        glViewport(0, 0, this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getWidth() * this->getRenderWindow()->getContentScaleFactor(), this->getRenderWindow()->getRenderView(BGERenderWindow::DefaultRenderViewName)->getHeight() * this->getRenderWindow()->getContentScaleFactor());
        
        std::shared_ptr<BGEShaderProgramOpenGLES2> glShader = std::dynamic_pointer_cast<BGEShaderProgramOpenGLES2>(pushShaderProgram("Default"));
        
        GLint positionLocation = glShader->locationForAttribute("Position");
        GLint colorLocation = glShader->locationForAttribute("SourceColor");
        GLint projectionLocation = glShader->locationForUniform("Projection");
        GLint modelLocation = glShader->locationForUniform("ModelView");
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(colorLocation);
        
        glUniformMatrix4fv(projectionLocation, 1, 0, (GLfloat *) projectionMatrix_.m);
        
        // 2
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
                              sizeof(BGEVertexColor), &Vertices[0]);
        glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(BGEVertexColor), (GLvoid*) (&Vertices[0].color));
        
        // 3
        glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]),
                       GL_UNSIGNED_BYTE, &Indices[0]);
        
        BGEVector2 position = { 0, 0 };
        BGEVector2 size = { 150, 300 };
        BGEVector4 colors[4] = {{ 1, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 1, 0, 1 }, { 0, 0, 1, 1 }};
        
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
            
            position = { 200, 1200 };
            f->drawString("HELLO YOU SACK OF DIRT AV abcDefg1210312084pdsoinjm_-'\"#*$&%", position, colors[1]);
        }
        
        [glContext->getContext() presentRenderbuffer:GL_RENDERBUFFER];
    }
}