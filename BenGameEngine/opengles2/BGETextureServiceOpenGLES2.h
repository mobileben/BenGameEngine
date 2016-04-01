//
//  BGETextureServiceOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureServiceOpenGLES2_h
#define BGETextureServiceOpenGLES2_h

#include <stdio.h>
#include "BGETextureService.h"
#include <GLKit/GLKit.h>

class BGETextureServiceOpenGLES2 : public BGETextureService
{
public:
    BGETextureServiceOpenGLES2(EAGLContext *context);
    ~BGETextureServiceOpenGLES2();
    
    virtual void initialize();
    virtual void reset();
    virtual void enteringBackground();
    virtual void enteringForeground();
    virtual void pause();
    virtual void resume();
    virtual void destroy();

    void namedTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError> error)> callback);
    void namedTextureFromURL(std::string name, std::string url, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>)> callback);
    void namedTextureFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>)> callback);
    
    void namedTextureAtlasFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGEError>)> callback);

    std::shared_ptr<BGETexture> namedSubTexture(std::string name, std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

private:
    GLKTextureLoader *textureLoader_;
};

#endif /* BGETextureServiceOpenGLES2_h */
