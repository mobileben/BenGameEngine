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
    class Texture;
    
    struct TextReferenceIntermediate {
        int32_t name;
        int32_t text;
        int32_t fontName;
        int32_t size;
        float width;
        float height;
        float leading;
        Color color;
        FontHorizontalAlignment alignment;
        bool multiline;
    };
    
    struct TextReference {
        const char *name;
        const char *text;
        float width;
        float height;
        float leading;
        Color color;
        FontHorizontalAlignment alignment;
        bool multiline;
        // TODO: Change this to Handle later. Note with this it is not a POD
        FontHandle fontHandle;
    };
    
    bool operator==(const TextReference& lhs, const TextReference& rhs);
    bool operator!=(const TextReference& lhs, const TextReference& rhs);
    bool operator==(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs);
    bool operator!=(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs);
    
    struct TextureReferenceIntermediate {
        int32_t         name;
        int32_t         atlasName;
        float           width;
        float           height;
        bool            isAtlas;
        TextureFormat   format;
    };
    
    struct TextureReference {
        const char* name;
        bool isAtlas;
        bool isSubTexture;
        TextureHandle textureHandle;
    };
    
    bool operator==(const TextureReference& lhs, const TextureReference& rhs);
    bool operator!=(const TextureReference& lhs, const TextureReference& rhs);
    bool operator==(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs);
    bool operator!=(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs);

    struct AnimationKeyframeReferenceIntermediate {
        uint32_t startFrame;
        uint32_t totalFrames;
        uint32_t order;
        uint32_t flags;
        uint32_t frame;
        int32_t position;
        int32_t scale;
        int32_t skew;
        float rotation;
        int32_t matrix;
        int32_t colorMatrix;
        int32_t colorTransform;
        int32_t bounds;
    };
    
    struct AnimationKeyframeReference {
        uint32_t startFrame;
        uint32_t totalFrames;
        uint32_t order;
        uint32_t flags;
        uint32_t frame;
        Vector2 *position;
        Vector2 *scale;
        Vector2 *skew;
        float  rotation;
        Matrix4 *matrix;
        ColorMatrix *colorMatrix;
        ColorTransform *colorTransform;
        Rect *bounds;
    };

    struct BoundsReference {
        uint32_t    startFrame;
        uint32_t    totalFrames;
        Rect        *bounds;
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
        GfxReferenceTypeKeyframe,
        GfxReferenceTypeText,
        GfxReferenceTypeTextureMask,
        GfxReferenceTypeLast
    } GfxReferenceType;
    
    struct AnimationChannelReferenceIntermediate {
        int32_t name;
        int32_t reference;
        GfxReferenceType referenceType;
        std::vector<int32_t> keyframes;
    };

    class AnimationSequenceReference;
    
    struct AnimationChannelReference {
        const char *name;
        const char *reference;
        GfxReferenceType referenceType;
        uint32_t numKeyframes;
        AnimationKeyframeReference **keyframes;
        union {
            void *referenceData;
            TextureReference *texture;
            TextReference *text;
            AnimationSequenceReference *sequence;
        };
        ScenePackageHandle scenePackage;
    };
    
    struct AnimationSequenceReferenceIntermediate {
        int32_t     name;
        uint32_t    frameRate;
        uint32_t    totalFrames;
        uint32_t    numChannels;
        uint32_t    numBounds;
        int32_t     channels;
        int32_t     bounds;
    };
    
    struct AnimationSequenceReference {
        const char *name;
        uint32_t frameRate;
        uint32_t totalFrames;
        uint32_t numChannels;
        uint32_t numBounds;
        AnimationChannelReference *channels;
        BoundsReference *bounds;
        ScenePackageHandle scenePackage;
    };
    
    struct ButtonStateIntermediate {
        int32_t             state;
        int32_t             reference;
        GfxReferenceType    referenceType;
    };
    
    struct ButtonStateReference {
        const char *state;
        const char *reference;
        GfxReferenceType referenceType;
    };
    
    struct ButtonIntermediate {
        int32_t     name;
        int32_t     states;
        uint32_t    numStates;
    };
    
    struct ButtonReference {
        const char              *name;
        ButtonStateReference    *states;
        uint32_t                numStates;
        ScenePackageHandle scenePackage;
    };
    
    struct PlacementIntermediate {
        int32_t name;
        float   width;
        float   height;
    };
    
    struct PlacementReference {
        const char *name;
        float       width;
        float       height;
    };
    
    struct MaskIntermediate {
        int32_t     name;
        float       width;
        float       height;
    };
    
    struct MaskReference {
        const char *name;
        float       width;
        float       height;
    };
    
    struct TextureMaskIntermediate {
        int32_t     name;
    };
    
    struct TextureMaskReference {
        const char *name;
        TextureReference *texture;
    };
    
    struct ExternalPackageIntermediate {
        int32_t name;
        int32_t externalPackage;
    };

    struct ExternalPackageReference {
        const char *name;
        const char *externalPackage;
    };
    
    struct AutoDisplayElementIntermediate {
        int32_t             name;
        int32_t             bounds;
        uint32_t            flags;
        int32_t             position;
        int32_t             scale;
        int32_t             skew;
        float               rotation;
        int32_t             matrix;
        int32_t             colorMatrix;
        int32_t             colorTransform;
        int32_t             reference;
        bool                hidden;
        GfxReferenceType    referenceType;
    };
    
    struct AutoDisplayElementReference {
        const char          *name;
        Rect                *bounds;
        uint32_t            flags;
        Vector2             *position;
        Vector2             *scale;
        Vector2             *skew;
        float               rotation;
        bool                hidden;
        Matrix4             *matrix;
        ColorMatrix         *colorMatrix;
        ColorTransform      *colorTransform;
        const char          *reference;
        GfxReferenceType    referenceType;
    };
}

#endif /* GraphicFormats_h */
