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
        float   width;
        float   height;
        float   leading;
        Color   color;
        FontHorizontalAlignment alignment;
        bool    multiline;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct TextReferenceIntermediate) == 13*4, "Check your assumptions");
    static_assert(std::is_pod<TextReferenceIntermediate>::value, "Must be a POD type.");

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

    struct SubTextureDefIntermediate {
        int32_t name;
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
        bool    rotated;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct SubTextureDefIntermediate) == 6*4, "Check your assumptions");
    static_assert(std::is_pod<SubTextureDefIntermediate>::value, "Must be a POD type.");

    struct TextureAtlasSubTextureDefIntermediate {
        int32_t name;
        int32_t numSubTextures;
        int32_t subTextDefStart;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct TextureAtlasSubTextureDefIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<TextureAtlasSubTextureDefIntermediate>::value, "Must be a POD type.");

    struct TextureReferenceIntermediate {
        int32_t         name;
        int32_t         filename;
        int32_t         atlasName;
        float           width;
        float           height;
        bool            isAtlas;
        TextureFormat   format;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct TextureReferenceIntermediate) == 7*4, "Check your assumptions");
    static_assert(std::is_pod<TextureReferenceIntermediate>::value, "Must be a POD type.");

    struct TextureReference {
        const char  *name;
        bool        isAtlas;
        bool        isSubTexture;
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
        int32_t collisionRectScale;
        float rotation;
        //int32_t matrix;
        int32_t colorMatrix;
        int32_t colorTransform;
        int32_t bounds;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct AnimationKeyframeReferenceIntermediate) == 13*4, "Check your assumptions");
    static_assert(std::is_pod<AnimationKeyframeReferenceIntermediate>::value, "Must be a POD type.");

    struct AnimationKeyframeReference {
        uint32_t startFrame;
        uint32_t totalFrames;
        uint32_t order;
        uint32_t flags;
        uint32_t frame;
        Vector2 *position;
        Vector2 *scale;
        Vector2 *skew;
        Vector2 *collisionRectScale;
        float  rotation;
        //Matrix4 *matrix;
        ColorMatrix *colorMatrix;
        ColorTransform *colorTransform;
        Rect *bounds;
    };

    bool operator==(const AnimationKeyframeReference& lhs, const AnimationKeyframeReference& rhs);
    bool operator!=(const AnimationKeyframeReference& lhs, const AnimationKeyframeReference& rhs);
    bool operator==(const AnimationKeyframeReferenceIntermediate& lhs, const AnimationKeyframeReferenceIntermediate& rhs);
    bool operator!=(const AnimationKeyframeReferenceIntermediate& lhs, const AnimationKeyframeReferenceIntermediate& rhs);

    struct BoundsReference {
        uint32_t    startFrame;
        uint32_t    totalFrames;
        Rect        *bounds;
    };
    
    struct BoundsReferenceIntermediate {
        uint32_t    startFrame;
        uint32_t    totalFrames;
        int32_t     bounds;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct BoundsReferenceIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<BoundsReferenceIntermediate>::value, "Must be a POD type.");

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
        uint32_t            chanId;
        int32_t             name;
        int32_t             reference;
        int32_t             startFrame;
        int32_t             order;
        GfxReferenceType    referenceType;
        int32_t             numKeyframes;
        int32_t             keyframesStart;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct AnimationChannelReferenceIntermediate) == 8*4, "Check your assumptions");
    static_assert(std::is_pod<AnimationChannelReferenceIntermediate>::value, "Must be a POD type.");

    struct AnimationChannelReferenceIntermediateKeyframes {
        uint32_t                chanId;
        std::vector<int32_t>    keyframes;
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
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct AnimationSequenceReferenceIntermediate) == 7*4, "Check your assumptions");
    static_assert(std::is_pod<AnimationSequenceReferenceIntermediate>::value, "Must be a POD type.");

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
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct ButtonStateIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<ButtonStateIntermediate>::value, "Must be a POD type.");

    struct ButtonStateReference {
        const char *state;
        const char *reference;
        GfxReferenceType referenceType;
    };
    
    struct ButtonIntermediate {
        int32_t     name;
        int32_t     states;
        uint32_t    numStates;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct ButtonIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<ButtonIntermediate>::value, "Must be a POD type.");

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
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct PlacementIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<PlacementIntermediate>::value, "Must be a POD type.");

    struct PlacementReference {
        const char *name;
        float       width;
        float       height;
    };
    
    struct MaskIntermediate {
        int32_t     name;
        float       width;
        float       height;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct MaskIntermediate) == 3*4, "Check your assumptions");
    static_assert(std::is_pod<MaskIntermediate>::value, "Must be a POD type.");

    struct MaskReference {
        const char *name;
        float       width;
        float       height;
    };
    
    struct TextureMaskIntermediate {
        int32_t     name;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct TextureMaskIntermediate) == 1*4, "Check your assumptions");
    static_assert(std::is_pod<TextureMaskIntermediate>::value, "Must be a POD type.");

    struct TextureMaskReference {
        const char *name;
        TextureReference *texture;
    };
    
    struct ExternalPackageIntermediate {
        int32_t name;
        int32_t externalPackage;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct ExternalPackageIntermediate) == 2*4, "Check your assumptions");
    static_assert(std::is_pod<ExternalPackageIntermediate>::value, "Must be a POD type.");

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
        int32_t             collisionRectScale;
        float               rotation;
        int32_t             colorMatrix;
        int32_t             colorTransform;
        int32_t             reference;
        bool                hidden;
        GfxReferenceType    referenceType;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct AutoDisplayElementIntermediate) == 13*4, "Check your assumptions");
    static_assert(std::is_pod<AutoDisplayElementIntermediate>::value, "Must be a POD type.");

    struct AutoDisplayElementReference {
        const char          *name;
        Rect                *bounds;
        uint32_t            flags;
        Vector2             *position;
        Vector2             *scale;
        Vector2             *skew;
        Vector2             *collisionRectScale;
        float               rotation;
        bool                hidden;
        ColorMatrix         *colorMatrix;
        ColorTransform      *colorTransform;
        const char          *reference;
        GfxReferenceType    referenceType;
    };
}

#endif /* GraphicFormats_h */
