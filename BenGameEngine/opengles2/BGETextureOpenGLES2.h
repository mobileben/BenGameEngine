//
//  BGETextureOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGETextureOpenGLES2_h
#define BGETextureOpenGLES2_h

#include <stdio.h>
#include "BGETexture.h"
#include <GLKit/GLKit.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

class BGETextureOpenGLES2 : public BGETexture
{
public:
    BGETextureOpenGLES2(std::string name);
    BGETextureOpenGLES2(std::string name, GLKTextureInfo *textureInfo);
    ~BGETextureOpenGLES2();
    
    GLKTextureInfo *getTextureInfo() const { return textureInfo_; }
    uint32_t getHWTextureId() const ;
    GLenum getTarget() const;
    
    void createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETexture>, std::shared_ptr<BGEError>)> callback);
    std::shared_ptr<BGEError> createSubTexture(std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

private:
    friend class BGETextureService;
    friend class BGETextureServiceOpenGLES2;
    
    GLKTextureInfo *textureInfo_;
    
    GLuint  hwId_;
    GLenum  target_;
    
    void createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
    void createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
    void createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
    void createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
    void createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
    void createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGEError>)> callback);
};

#endif /* BGETextureOpenGLES2_h */
