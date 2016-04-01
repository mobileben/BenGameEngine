//
//  BGETextureAtlasOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureAtlasOpenGLES2_h
#define BGETextureAtlasOpenGLES2_h

#include <stdio.h>
#include "BGETextureAtlas.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

class BGETextureAtlasOpenGLES2 : public BGETextureAtlas
{
public:
    BGETextureAtlasOpenGLES2(std::string name);
    ~BGETextureAtlasOpenGLES2();
    
    uint32_t getHWTextureId() const;
    GLenum getTarget() const;

    void createFromBuffer(void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextures, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGEError>)> callback);

private:
    friend class BGETextureService;
    friend class BGETextureServiceOpenGLES2;

};

#endif /* BGETextureAtlasOpenGLES2_h */
