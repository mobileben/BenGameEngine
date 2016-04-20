//
//  TextureOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureOpenGLES2_h
#define BGETextureOpenGLES2_h

#include <stdio.h>
#include "Texture.h"
#include <GLKit/GLKit.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace BGE {
    class TextureOpenGLES2 : public Texture
    {
    public:
        TextureOpenGLES2(uint64_t texId, std::string name);
        TextureOpenGLES2(uint64_t texId, std::string name, GLKTextureInfo *textureInfo);
        ~TextureOpenGLES2();
        
        GLKTextureInfo *getTextureInfo() const { return textureInfo_; }
        uint32_t getHWTextureId() const ;
        GLenum getTarget() const;
        
        void createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<Texture>, std::shared_ptr<BGE::Error>)> callback);
        std::shared_ptr<BGE::Error> createSubTexture(std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        
    private:
        friend class TextureService;
        friend class TextureServiceOpenGLES2;
        
        GLKTextureInfo *textureInfo_;
        
        GLuint  hwId_;
        GLenum  target_;
        
        void createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
        void createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
        void createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
        void createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
        void createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
        void createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGE::Error>)> callback);
    };
}

#endif /* BGETextureOpenGLES2_h */
