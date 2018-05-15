//
//  TextureAtlas.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/18/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETextureAtlas_h
#define BGETextureAtlas_h

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "Texture.h"

namespace BGE {
    typedef struct {
        std::string name;
        int         x;
        int         y;
        int         width;
        int         height;
        bool        rotated;
    } SubTextureDef;
    
    class TextureAtlas : public NamedObject
    {
    public:
        static const std::string ErrorDomain;
        
        TextureAtlas();
        TextureAtlas(ObjectId fontId);
        TextureAtlas(ObjectId objId, std::string name);
        ~TextureAtlas() {}
        
        void initialize(TextureAtlasHandle handle, std::string name);
        void destroy();
        
        inline bool isValid() const { return valid_; }

        inline TextureAtlasHandle getHandle() const { return handle_; }
        inline TextureHandle getTextureHandle() const { return textureHandle_; };
        
        inline uint32_t getHWTextureId() const {
            return hwId_;
        }
        
        inline GLenum getTarget() const {
            return target_;
        }
        
        inline TextureAlphaState getAlphaState() const { return alphaState_; }
        inline TextureFormat getFormat() const { return format_; }

        inline uint32_t getWidth() const {
            return width_;
        }
        
        inline uint32_t getHeight() const {
            return height_;
        }
        
        std::string atlasTextureKey() const;
        TextureHandle getSubTextureHandle(std::string name);
        Texture *getSubTexture(std::string name);
        const std::map<std::string, TextureHandle>& getSubTextures() const;

        size_t getMemoryUsage() const;

        std::pair<TextureAtlas *, std::shared_ptr<Error>> createFromFile(std::string filename, std::vector<SubTextureDef> &subTextures, TextureFormat format);
        std::pair<TextureAtlas *, std::shared_ptr<Error>> createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextures);
        
    private:
        // TODO: valid_ needs to become state or status
        bool                    valid_;
        TextureAtlasHandle      handle_;
        TextureHandle           textureHandle_;
        
        uint32_t                width_;
        uint32_t                height_;
        
        GLuint                  hwId_;
        GLenum                  target_;
        
        TextureFormat           format_;
        TextureAlphaState       alphaState_;
        
        std::map<std::string, TextureHandle> subTextures_;
        
        void reset();
    };
}

#endif /* BGETextureAtlas_h */
