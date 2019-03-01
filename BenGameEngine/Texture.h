//
//  Texture.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETexture_h
#define BGETexture_h

#include <stdio.h>

#include <string>

#include "Error.h"
#include "NamedObject.h"
#include "MathTypes.h"
#include "Handle.h"
#include "RawTexture.h"
#ifdef SUPPORT_GLKTEXTURELOADER
#include <GLKit/GLKit.h>
#endif /* SUPPORT_GLKTEXTURELOADER */

#ifdef SUPPORT_OPENGL
#ifdef SUPPORT_OPENGLES2
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif /* SUPPORT_OPENGLES2 */
#endif /* SUPPORT_OPENGL */

#include <vector>
#include <map>

namespace BGE {
    typedef enum {
        TextureErrorNone = 0,
        TextureErrorUnsupported,
        TextureErrorOS,
        TextureErrorNoBuffer,
        TextureErrorZeroWidthOrHeight,
        TextureErrorClassAllocation,
        TextureErrorAllocation,
        TextureErrorInvalidSubTexture,
        TextureErrorExistingTextureWrongType,
        TextureErrorVboAllocation
    } TextureError;
    
    enum class TextureState : uint32_t {
        Unititialized,
        Loading,
        Loaded,
        Error
    };
    
    enum class TextureAlphaState {
        None,
        NonPremultiplied,
        Premultiplied
    };
    
    enum class TextureFormat : uint32_t {
        Undefined = 0,
        Alpha,
        RGB565,
        RGB888,
        RGBA5551,
        RGBA4444,
        RGBA8888
    };
    
    struct VertexTex {
        Vector3 position;
        Vector2 tex;
    };

    class TextureAtlas;
    
    class Texture : public NamedObject
    {
    public:
        static const std::string ErrorDomain;

        Texture();
#ifdef NOT_YET
        Texture(ObjectId texId);
        Texture(ObjectId texId, std::string name);
#ifdef SUPPORT_GLKTEXTURELOADER
        Texture(uint32_t texId, std::string name, GLKTextureInfo *textureInfo);
#endif /* SUPPORT_GLKTEXTURELOADER */
#endif
        ~Texture() {}
        
        void initialize(TextureHandle handle, std::string name, TextureFormat format);
        std::shared_ptr<Error> initialize(TextureHandle handle, std::string name, RawTexture *rawTexture);
#ifdef SUPPORT_GLKTEXTURELOADER
        void initialize(TextureHandle handle, std::string name, TextureFormat format, GLKTextureInfo *texInfo);
#endif /* SUPPORT_GLKTEXTURELOADER */
        void destroy();
        
        inline TextureAlphaState getAlphaState() const { return alphaState_; }
        inline TextureFormat getFormat() const { return format_; }
        
        inline bool isValid() const { return valid_; }

        inline TextureHandle getHandle() const { return handle_; }
        
        inline uint32_t getX() const { return x_; }
        inline uint32_t getY() const { return y_; }
        inline uint32_t getWidth() const { return width_; }
        inline uint32_t getHeight() const { return height_; }
        
        inline bool isSubTexture() const { return isSubTexture_; }
        
#ifdef SUPPORT_GLKTEXTURELOADER
        inline GLKTextureInfo *getTextureInfo() const { return textureInfo_; }
#endif /* SUPPORT_GLKTEXTURELOADER */
#ifdef SUPPORT_OPENGL
        inline uint32_t getHWTextureId() const { return hwId_; }
        inline uint32_t getHWVboId() const { return vboId_; }
        inline uint32_t getHWIboId() const { return iboId_; }
        inline GLushort *getVboIndices() const { return const_cast<GLushort *>(&vboIndices_[0]); }
        inline GLsizei getVboIndicesSize() const { return static_cast<GLsizei>(sizeof(vboIndices_)); }
        inline GLsizei getVboIndicesCount() const { return static_cast<GLsizei>(sizeof(vboIndices_)/sizeof(vboIndices_[0])); }
        inline GLenum getVboIndexType() const { return static_cast<GLenum>(GL_UNSIGNED_SHORT); }
        inline GLenum getTarget() const { return target_; }
#endif /* SUPPORT_OPENGL */
        inline size_t getMemoryUsage() const { return memoryUsage_; }
        
        std::shared_ptr<Error> createFromBuffer(void *buffer, TextureFormat format, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createSubTexture(TextureAtlas *atlas, uint32_t x, uint32_t y, uint32_t width, uint32_t height, bool rotated, bool font);
        
        inline TextureAtlasHandle getTextureAtlasHandle() { return atlasHandle_; }
        
        inline const Vector2 *getXYs() const { return &xys_[0]; }
        
        inline float getX0() const { return xys_[0].x; }
        inline float getY0() const { return xys_[0].y; }
        inline float getX1() const { return xys_[1].x; }
        inline float getY1() const { return xys_[1].y; }
        inline float getX2() const { return xys_[2].x; }
        inline float getY2() const { return xys_[2].y; }
        inline float getX3() const { return xys_[3].x; }
        inline float getY4() const { return xys_[3].y; }
        
        inline const Vector2 *getUVs() const { return &uvs_[0]; }
        
        inline float getU0() const { return uvs_[0].x; }
        inline float getV0() const { return uvs_[0].y; }
        inline float getU1() const { return uvs_[1].x; }
        inline float getV1() const { return uvs_[1].y; }
        inline float getU2() const { return uvs_[2].x; }
        inline float getV2() const { return uvs_[2].y; }
        inline float getU3() const { return uvs_[3].x; }
        inline float getV4() const { return uvs_[3].y; }
        
    private:
        friend class TextureService;
        friend class TextureAtlas;
        
        // TODO: valid_ needs to become state or status
        bool                valid_;
        TextureHandle       handle_;
#ifdef SUPPORT_OPENGL
        GLuint              hwId_;
        GLenum              target_;
        GLuint              vboId_;
        GLuint              iboId_;
        std::vector<VertexTex>  vertexTexData_;
        GLushort            vboIndices_[6];
#endif /* SUPPORT_OPENGL */
        Vector2             xys_[4];
        Vector2             uvs_[4];

        uint32_t            width_;
        uint32_t            height_;
        uint32_t            x_;
        uint32_t            y_;
        
        size_t              memoryUsage_;
        
        bool                isSubTexture_;
        TextureAtlasHandle  atlasHandle_;
        
        TextureFormat       format_;
        TextureAlphaState   alphaState_;
#ifdef SUPPORT_GLKTEXTURELOADER
        GLKTextureInfo      *textureInfo_;
#endif /* SUPPORT_GLKTEXTURELOADER */

        void setValid(bool valid) { valid_ = valid; }
        void setFormat(TextureFormat format) { format_ = format; }
        void setAlphaState(TextureAlphaState alphaState) { alphaState_ = alphaState; }
        void setX(uint32_t x) { x_ = x; }
        void setY(uint32_t y) { y_ = y; }
        void setWidth(uint32_t width) { width_ = width; }
        void setHeight(uint32_t height) { height_ = height; }
        
        void updateUVs(bool rotated = false);
        void updateXYs();
        void updateXYsForFont();

        std::shared_ptr<Error> createTextureFromAlphaBuffer(unsigned char *buffer, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createTextureFromRGB565Buffer(unsigned char *buffer, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createTextureFromRGB888Buffer(unsigned char *buffer, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createTextureFromRGBA5551Buffer(unsigned char *buffer, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createTextureFromRGBA4444Buffer(unsigned char *buffer, uint32_t width, uint32_t height);
        std::shared_ptr<Error> createTextureFromRGBA8888Buffer(unsigned char *buffer, uint32_t width, uint32_t height);
        
        // This will do a std::move
        void moveToVertexTexData(std::vector<VertexTex>& data);
        size_t computeMemoryUsage(TextureFormat format, uint32_t width, uint32_t height);
    };
}

#endif /* BGETexture_h */
