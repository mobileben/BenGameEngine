//
//  TextureServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureServiceOpenGLES2_h
#define BGETextureServiceOpenGLES2_h

#include <stdio.h>
#include "TextureService.h"
#include <GLKit/GLKit.h>

namespace BGE {
    class TextureServiceOpenGLES2 : public TextureService
    {
    public:
        TextureServiceOpenGLES2(EAGLContext *context);
        ~TextureServiceOpenGLES2();
        
        virtual void initialize();
        virtual void reset();
        virtual void enteringBackground();
        virtual void enteringForeground();
        virtual void pause();
        virtual void resume();
        virtual void destroy();
        void update(double deltaTime) {}

        void namedTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error> error)> callback);
        void namedTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error> error)> callback);
        void namedTextureFromURL(std::string name, std::string url, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback);
        void namedTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<TextureBase>, std::shared_ptr<Error>)> callback);
        
        void namedTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback);
        
        std::shared_ptr<Texture> namedSubTexture(std::string name, std::shared_ptr<TextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated = false);
        
    private:
        GLKTextureLoader *textureLoader_;
    };
}

#endif /* BGETextureServiceOpenGLES2_h */
