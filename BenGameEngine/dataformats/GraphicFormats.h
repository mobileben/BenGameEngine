//
//  GraphicFormats.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef GraphicFormats_h
#define GraphicFormats_h

#include <stdio.h>
#include <cstdint>
#include "MathTypes.h"
#include "Font.h"

namespace BGE {
    struct AnimationKeyFrameDataFormat {
        uint32_t totalFrames;
        Matrix3 matrix;
        ColorMatrix colorMatrix;
        uint32_t flags;
        Rect bounds;
    };
    
    struct AnimationChannelDataFormat {
        const char* const reference;
        const char* const referenceType;
        uint32_t totalFrames;
        AnimationKeyFrameDataFormat *keyframes;
    };
    
    struct AnimationDataFormat {
        char *name;
        uint32_t fps;
        uint32_t totalFrames;
        uint32_t numChannels;
        AnimationChannelDataFormat *channels;
    };
    
    struct MaskDataFormat {
        const char* const name = nullptr;
        float width = 0;
        float height = 0;
    };
    
    struct ExternalReferenceDataFormat {
        const char* const name;
        const char* const externalPackage;
    };
    
    struct AutoDisplayListElementDataFormat {
        const char* const name;
        const char* const referenceType;
        uint32_t flags;
        uint32_t hidden;
        Rect bounds;
        Vector2 xy;
        Vector2 scale;
        float rotation;
        const char* const reference;
    };
    
    struct PlacementDataFormat {
        const char* const name;
        float width;
        float height;
    };
    
    struct TextDataFormat {
        enum Alignment {
            Left = 0,
            Center = 1,
            Right = 2
        };
        
        const char* const name = nullptr;
        const char* const text = nullptr;
        const char* const fontName = nullptr;
        Alignment alignment;
        uint32_t color = 0;
        float size = 0;
        float leading = 0;
        
        float width = 0;
        float height = 0;
    };
    
    struct SubTextureDataFormat {
        const char* const name;
        const char* const atlas;
        float rotation;
        float x;
        float y;
        float width;
        float height;
        
    };
    
    struct TextureDataFormat {
        const char* const name = nullptr;
        const char* const filename = nullptr;
        float   width = 0;
        float   height = 0;
    };

    class Texture;
    
    struct AnimationKeyFrameReference {
        uint32_t startFrame;
        uint32_t totalFrames;
        uint32_t order;
        uint32_t flags;
        Vector2 position;
        Vector2 scale;
        float  rotation;
        Matrix3 matrix;
        ColorMatrix colorMatrix;
        ColorTransform colorTransform;
        Rect bounds;
    };
    
    struct AnimationKeyFrameReferenceIntermediate {
        uint32_t startFrame;
        uint32_t totalFrames;
        uint32_t order;
        uint32_t flags;
        int32_t position;
        int32_t scale;
        float rotation;
        int32_t matrix;
        int32_t colorMatrix;
        int32_t colorTransform;
        int32_t bounds;
    };

    struct BoundsReference {
        uint32_t startFrame;
        uint32_t totalFrames;
        Rect bounds;
    };
    
    struct BoundsReferenceIntermediate {
        uint32_t    startFrame;
        uint32_t    totalFrames;
        int32_t     bounds;
    };
    
    typedef enum : uint32_t {
        GfxReferenceTypeUnknown = 0,
        GfxReferenceTypeButton,
        GfxReferenceTypeExternalReference,
        GfxReferenceTypeMask,
        GfxReferenceTypePlacement,
        GfxReferenceTypeSprite,
        GfxReferenceTypeAnimationSequence,
        GfxReferenceTypeKeyFrame,
        GfxReferenceTypeText,
        GfxReferenceTypeTextureMask
    } GfxReferenceType;

    struct AnimationChannelReference {
        const char *name;
        const char *reference;
        GfxReferenceType referenceType;
        uint32_t numKeyFrames;
        AnimationKeyFrameReference **keyframes;
    };

    struct AnimationChannelReferenceIntermediate {
        const char *name;
        const char *reference;
        GfxReferenceType referenceType;
        std::vector<int32_t> keyFrames;
    };
    
    struct AnimationSequenceReference {
        const char *name;
        uint32_t frameRate;
        uint32_t totalFrames;
        uint32_t numChannels;
        uint32_t numBounds;
        AnimationChannelReference *channels;
        BoundsReference *bounds;
    };
    
    struct AnimationSequenceReferenceIntermediate {
        const char *name;
        uint32_t frameRate;
        uint32_t totalFrames;
        uint32_t numChannels;
        uint32_t numBounds;
        uint32_t channels;
        uint32_t bounds;
    };
    
    struct TextReference {
        const char* name;
        float width;
        float height;
        float leading;
        Color color;
        FontHorizontalAlignment alignment;
        Font* font;
    };

    struct TextReferenceIntermediate {
        int32_t name;
        float width;
        float height;
        float leading;
        Color color;
        FontHorizontalAlignment alignment;
    };
    
    bool operator==(const TextReference& lhs, const TextReference& rhs);
    bool operator!=(const TextReference& lhs, const TextReference& rhs);
    bool operator==(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs);
    bool operator!=(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs);

    struct TextureReference {
        const char* name;
        float   width;
        float   height;
        // TODO: Handle
        TextureBase *texture;
    };
    
    struct TextureReferenceIntermediate {
        int32_t name;
        float   width;
        float   height;
    };
    
    bool operator==(const TextureReference& lhs, const TextureReference& rhs);
    bool operator!=(const TextureReference& lhs, const TextureReference& rhs);
    bool operator==(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs);
    bool operator!=(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs);
}

#endif /* GraphicFormats_h */
