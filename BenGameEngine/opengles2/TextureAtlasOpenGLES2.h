//
//  TextureAtlasOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureAtlasOpenGLES2_h
#define BGETextureAtlasOpenGLES2_h

#include <stdio.h>
#include "TextureAtlas.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace BGE {
    class TextureAtlasOpenGLES2 : public TextureAtlas
    {
    public:
        TextureAtlasOpenGLES2(uint32_t texId, std::string name);
        ~TextureAtlasOpenGLES2();
        
        uint32_t getHWTextureId() const;
        GLenum getTarget() const;
        
        void createFromFile(std::string filename, std::vector<SubTextureDef> &subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback);
        void createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextures, std::function<void(std::shared_ptr<TextureAtlas>, std::shared_ptr<Error>)> callback);
        
    private:
        friend class TextureService;
        friend class TextureServiceOpenGLES2;
        
    };
}

#endif /* BGETextureAtlasOpenGLES2_h */
