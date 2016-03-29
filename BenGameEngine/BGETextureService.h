//
//  BGETextureService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/19/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGETextureService_h
#define BGETextureService_h

#include <stdio.h>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "BGEService.h"
#include "BGETextureBase.h"
#include "BGETexture.h"
#include "BGETextureAtlas.h"
#include "BGEError.h"

class BGETextureService : public BGEService
{
public:
    BGETextureService();
    virtual ~BGETextureService() {}
    
    virtual void namedTextureFromFile(std::string name, std::string filename, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>)> callback) =0;
    virtual void namedTextureFromURL(std::string name, std::string url, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>)> callback) =0;
    virtual void namedTextureFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::function<void(std::shared_ptr<BGETextureBase>, std::shared_ptr<BGEError>)> callback) =0;
    
    virtual void namedTextureAtlasFromBuffer(std::string name, void *buffer, BGETextureFormat format, uint32_t width, uint32_t height, std::map<std::string, BGESubTextureDef> subTextureDefs, std::function<void(std::shared_ptr<BGETextureAtlas>, std::shared_ptr<BGEError>)> callback) =0;
    
    virtual std::shared_ptr<BGETexture> namedSubTexture(std::string name, std::shared_ptr<BGETextureAtlas> atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height) =0;
    
    void removeTexture(std::string name);
    void removeTexture(std::shared_ptr<BGETextureBase>);
    
    std::shared_ptr<BGETextureBase> textureWithName(std::string name);
    
protected:
    std::map<std::string, std::shared_ptr<BGETextureBase>> textures_;
};

#endif /* BGETextureService_h */
