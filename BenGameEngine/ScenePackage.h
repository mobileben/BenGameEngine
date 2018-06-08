//
//  ScenePackage.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ScenePackage_h
#define ScenePackage_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#include <stdio.h>
#include <vector>
#include <atomic>
#include <functional>
#include "NamedObject.h"
#include "Handle.h"
#include "GraphicFormats.h"
#include "FixedArray.h"
#include "MathTypes.h"
#include "FileUtilities.h"

#ifdef NOT_YET
namespace BGE {
    struct RapidJsonException : public std::exception {
        virtual const char *what() const throw() {
            return "rapidjson exception";
        }
    };
}

#ifdef RAPIDJSON_ASSERT
#undef RAPIDJSON_ASSERT
#endif

#define RAPIDJSON_ASSERT(x) if (!(x)) throw BGE::RapidJsonException();
#endif

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace BGE {
    extern const char *kScenePackageKeyAlignment;
    extern const char *kScenePackageKeyAlphaMultiplier;
    extern const char *kScenePackageKeyAlphaOffset;
    extern const char *kScenePackageKeyAtlas;
    extern const char *kScenePackageKeyAutoDisplayList;
    extern const char *kScenePackageKeyBlueMultiplier;
    extern const char *kScenePackageKeyBlueOffset;
    extern const char *kScenePackageKeyBounds;
    extern const char *kScenePackageKeyButtons;
    extern const char *kScenePackageKeyCenter;
    extern const char *kScenePackageKeyChildren;
    extern const char *kScenePackageKeyCollRectScale;
    extern const char *kScenePackageKeyColor;
    extern const char *kScenePackageKeyColorMatrixFilter;
    extern const char *kScenePackageKeyExternalPackage;
    extern const char *kScenePackageKeyExternalReferences;
    extern const char *kScenePackageKeyFilename;
    extern const char *kScenePackageKeyFilters;
    extern const char *kScenePackageKeyFlags;
    extern const char *kScenePackageKeyFontColor;
    extern const char *kScenePackageKeyFontName;
    extern const char *kScenePackageKeyFontSize;
    extern const char *kScenePackageKeyFormat;
    extern const char *kScenePackageKeyFrame;
    extern const char *kScenePackageKeyFrameRate;
    extern const char *kScenePackageKeyFrames;
    extern const char *kScenePackageKeyGreenMultiplier;
    extern const char *kScenePackageKeyGreenOffset;
    extern const char *kScenePackageKeyHeight;
    extern const char *kScenePackageKeyHidden;
    extern const char *kScenePackageKeyLeading;
    extern const char *kScenePackageKeyLeft;
    extern const char *kScenePackageKeyMasks;
    extern const char *kScenePackageKeyMatrix;
    extern const char *kScenePackageKeyMultiline;
    extern const char *kScenePackageKeyName;
    extern const char *kScenePackageKeyPixelsPerPoint;
    extern const char *kScenePackageKeyPlacements;
    extern const char *kScenePackageKeyPosition;
    extern const char *kScenePackageKeyRedMultiplier;
    extern const char *kScenePackageKeyRedOffset;
    extern const char *kScenePackageKeyReference;
    extern const char *kScenePackageKeyReferenceType;
    extern const char *kScenePackageKeyRight;
    extern const char *kScenePackageKeyRotated;
    extern const char *kScenePackageKeyRotation;
    extern const char *kScenePackageKeyScale;
    extern const char *kScenePackageKeySkew;
    extern const char *kScenePackageKeySource;
    extern const char *kScenePackageKeyState;
    extern const char *kScenePackageKeyStates;
    extern const char *kScenePackageKeySubtextures;
    extern const char *kScenePackageKeySymbols;
    extern const char *kScenePackageKeyText;
    extern const char *kScenePackageKeyTextureMasks;
    extern const char *kScenePackageKeyTextures;
    extern const char *kScenePackageKeyWidth;
    extern const char *kScenePackageKeyX;
    extern const char *kScenePackageKeyY;

    enum class ScenePackageStatus {
        Invalid,
        Loading,
        Valid,
        Linked
    };
    
    enum class ScenePackageError : int32_t {
        None = 0,
        UnsupportedFormat,
        DoesNotExist,
        Loading,
        NoAvailableHandles
    };

    class ScenePackage;

    const int32_t   kScenePackageFormatHeaderMagicSize = 8;
    const uint32_t  kScenePackageFormatVersion = 0;
    const uint32_t  kScenePackageFormatEndian = 1;
    extern const uint8_t   kScenePackageFormatMagic[kScenePackageFormatHeaderMagicSize];

    struct ScenePackageFormatHeader {
        uint8_t         magic[kScenePackageFormatHeaderMagicSize];
        uint32_t        endian;
        uint32_t        version;
        float           frameRate;
        float           pixelsPerPoint;
        float           width;
        float           height;
        Vector2         position;

        int32_t         emptyStringIndex;
        int32_t         defaultPositionIndex;
        int32_t         defaultScaleIndex;
        int32_t         defaultSkewIndex;
        int32_t         defaultCollisionRectScaleIndex;

        int32_t         sourceIndex;

        uint32_t        stringsOffset;
        uint32_t        stringsSize;
        uint32_t        atlasSubtexturesOffset;
        uint32_t        atlasSubtexturesSize;
        uint32_t        subTextureDefsOffset;
        uint32_t        subTextureDefsSize;
        uint32_t        texturesOffset;
        uint32_t        texturesSize;
        uint32_t        textOffset;
        uint32_t        textSize;
        uint32_t        animationSequencesOffset;
        uint32_t        animationSequencesSize;
        uint32_t        placementsOffset;
        uint32_t        placementsSize;
        uint32_t        masksOffset;
        uint32_t        masksSize;
        uint32_t        textureMasksOffset;
        uint32_t        textureMasksSize;
        uint32_t        buttonsOffset;
        uint32_t        buttonsSize;
        uint32_t        buttonStatesOffset;
        uint32_t        buttonStatesSize;
        uint32_t        externalPackagesOffset;
        uint32_t        externalPackagesSize;
        uint32_t        autoDisplayElementsOffset;
        uint32_t        autoDisplayElementsSize;
        uint32_t        rectsOffset;
        uint32_t        rectsSize;
        uint32_t        colorTransformsOffset;
        uint32_t        colorTransformsSize;
        uint32_t        colorMatricesOffset;
        uint32_t        colorMatricesSize;
        uint32_t        vector2sOffset;
        uint32_t        vector2sSize;
        uint32_t        keyframeIndicesOffset;
        uint32_t        keyframeIndicesSize;
        uint32_t        boundsOffset;
        uint32_t        boundsSize;
        uint32_t        keyframesOffset;
        uint32_t        keyframesSize;
        uint32_t        channelsOffset;
        uint32_t        channelsSize;
    } __attribute__((aligned(4)));

    static_assert(sizeof(struct ScenePackageFormatHeader) == 58*4, "Check your assumptions");
    static_assert(std::is_pod<ScenePackageFormatHeader>::value, "Must be a POD type.");

    struct ScenePackageFormat {
        float                                                   frameRate_;
        float                                                   pixelsPerPoint_;
        float                                                   width_;
        float                                                   height_;
        Vector2                                                 position_;

        int32_t                                                 emptyStringIndex_;
        int32_t                                                 defaultPositionIndex_;
        int32_t                                                 defaultScaleIndex_;
        int32_t                                                 defaultSkewIndex_;
        int32_t                                                 defaultCollisionRectScaleIndex_;

        int32_t                                                 sourceIndex_;

        FixedArray<char>                                        strings_;
        FixedArray<TextureAtlasSubTextureDefIntermediate>       atlasSubtextures_;
        FixedArray<SubTextureDefIntermediate>                   subTextureDefs_;
        FixedArray<TextureReferenceIntermediate>                textures_;
        FixedArray<TextReferenceIntermediate>                   text_;
        FixedArray<AnimationSequenceReferenceIntermediate>      animationSequences_;
        FixedArray<PlacementIntermediate>                       placements_;
        FixedArray<MaskIntermediate>                            masks_;
        FixedArray<TextureMaskIntermediate>                     textureMasks_;
        FixedArray<ButtonIntermediate>                          buttons_;
        FixedArray<ButtonStateIntermediate>                     buttonStates_;
        FixedArray<ExternalPackageIntermediate>                 externalPackages_;
        FixedArray<AutoDisplayElementIntermediate>              autoDisplayElements_;

        FixedArray<Rect>                                        rects_;
        FixedArray<ColorTransform>                              colorTransforms_;
        FixedArray<ColorMatrix>                                 colorMatrices_;
        FixedArray<Vector2>                                     vector2s_;
        FixedArray<int32_t>                                     keyframeIndices_;

        FixedArray<BoundsReferenceIntermediate>                 bounds_;
        FixedArray<AnimationKeyframeReferenceIntermediate>      keyframes_;
        FixedArray<AnimationChannelReferenceIntermediate>       channels_;

        bool                                                    managed_;
        ScenePackageFormatHeader                                *header_;
        uint64_t                                                *buffer_;
        size_t                                                  bufferSize_;

        ScenePackageFormat();
        ScenePackageFormat(const ScenePackage& package);
        ScenePackageFormat(const uint64_t *buffer, size_t bufferSize, bool managed = true);
        ~ScenePackageFormat();

        bool isAligned8Memory(size_t size) const;
        size_t aligned8MemorySize(size_t size) const;

        void load(const uint64_t *buffer, size_t bufferSize, bool managed = true);
        void save(const std::string& filename);
    };

    class ScenePackage : public NamedObject {
    public:
        static const std::string ErrorDomain;
        
        ScenePackage();
        ScenePackage(ObjectId sceneId);
        ~ScenePackage() {}
        
        void initialize(ScenePackageHandle handle, std::string name);
        void destroy();
        
        size_t getMemoryUsage() const;
        
        void outputResourceBreakdown(uint32_t numTabs) const;
        void outputMemoryBreakdown(uint32_t numTabs) const;

        void link();
        
        void addSpaceHandleReference(SpaceHandle spaceHandle);
        
        bool hasExternalPackages() const {
            return hasExternal_;
        }
        
        inline bool isValid() const { return status_ >= ScenePackageStatus::Valid; }
        inline ScenePackageHandle getHandle() const { return handle_; }
        
        BaseDirectory getBaseDirectory() const;
        void setBaseDirectory(const BaseDirectory &baseDirectory);
        
        AutoDisplayElementReference *getAutoDisplayList() const;
        int32_t getAutoDisplayListSize() const;
        
        AnimationSequenceReference *getAnimationSequenceReference(std::string name);
        ButtonReference *getButtonReference(std::string name);
        MaskReference *getMaskReference(std::string name);
        TextureMaskReference *getTextureMaskReference(std::string name);
        PlacementReference *getPlacementReference(std::string name);
        TextReference *getTextReference(std::string name);
        TextureReference *getTextureReference(std::string name);
        ExternalPackageReference *getExternalReference(std::string name);
        bool hasSubTexture(std::string name);
        
        // Get the reference type for the item, if it exists
        GfxReferenceType getReferenceType(std::string name);

        std::string textureFilename(const std::string& name);

        // This is for debugging
        void saveAsBinary(const std::string& filename);

    protected:
        void setStatus(ScenePackageStatus status) { status_ = status; }
        void create(const ScenePackageFormat& format, std::function<void(ScenePackage *)> callback);
        void create(const uint64_t *buffer, size_t bufferSize, bool managed, std::function<void(ScenePackage *)> callback);
        void create(const std::shared_ptr<rapidjson::Document> jsonDict, std::function<void(ScenePackage *)> callback);
        void loadAllTextures(std::function<void()> callback);

    private:
        friend class ScenePackageService;
        friend class ScenePackageFormat;
        
        struct TextureQueueItem {
            std::string     name;
            FilePath        filePath;
            TextureFormat   format;
        };
        
        ScenePackageStatus                                  status_;
        ScenePackageHandle                                  handle_;

        std::shared_ptr<ScenePackageFormat>                 format_;
        
        BaseDirectory                                       baseDirectory_;
        
        std::string                                         source_;
        float                                               frameRate_;
        float                                               pixelsPerPoint_;
        float                                               width_;
        float                                               height_;
        Vector2                                             position_;

        int32_t                                             emptyStringIndex_;
        int32_t                                             defaultPositionIndex_;
        int32_t                                             defaultScaleIndex_;
        int32_t                                             defaultSkewIndex_;
        int32_t                                             defaultCollisionRectScaleIndex_;

        int32_t                                             sourceIndex_;

        bool                                                fontsLoaded_;
        bool                                                texturesLoaded_;
        bool                                                hasExternal_;

        size_t                                              memoryUsage_;

        // Binary Data -- Start --

        FixedArray<char>                                    strings_;
        FixedArray<TextureAtlasSubTextureDefIntermediate>   atlasSubtextures_;
        FixedArray<SubTextureDefIntermediate>               subTextureDefs_;
        FixedArray<TextureReferenceIntermediate>            textures_;
        FixedArray<TextReferenceIntermediate>               text_;
        FixedArray<AnimationSequenceReferenceIntermediate>  animationSequences_;
        FixedArray<PlacementIntermediate>                   placements_;
        FixedArray<MaskIntermediate>                        masks_;
        FixedArray<TextureMaskIntermediate>                 textureMasks_;
        FixedArray<ButtonIntermediate>                      buttons_;
        FixedArray<ButtonStateIntermediate>                 buttonStates_;
        FixedArray<ExternalPackageIntermediate>             externalPackages_;
        FixedArray<AutoDisplayElementIntermediate>          autoDisplayElements_;
        
        FixedArray<Rect>                                    rects_;
        FixedArray<ColorTransform>                          colorTransforms_;
        FixedArray<ColorMatrix>                             colorMatrices_;
        FixedArray<Vector2>                                 vector2s_;
        FixedArray<int32_t>                                 keyframeIndices_;
        
        FixedArray<BoundsReferenceIntermediate>             bounds_;
        FixedArray<AnimationKeyframeReferenceIntermediate>  keyframes_;
        FixedArray<AnimationChannelReferenceIntermediate>   channels_;

        // Binary Data -- End --

        std::unordered_map<std::string, std::vector<SubTextureDef>> subTextures_;
        
        std::vector<TextureQueueItem>                       textureQueue_;
        std::vector<std::pair<std::string, uint32_t>>       fontQueue_;
        
        // Linked versions
        FixedArray<const char*>                             textureNames_;
        FixedArray<int32_t>                                 textureIndices_;
        FixedArray<TextureReference>                        textureRefs_;

        FixedArray<const char*>                             textNames_;
        FixedArray<int32_t>                                 textIndices_;
        FixedArray<TextReference>                           textRefs_;
 
        FixedArray<BoundsReference>                         boundsRefs_;

        FixedArray<const char*>                             animSeqNames_;
        FixedArray<int32_t>                                 animSeqIndices_;
        FixedArray<AnimationSequenceReference>              animSeqRefs_;
        
        FixedArray<const char*>                             animChannelNames_;
        FixedArray<int32_t>                                 animChannelIndices_;
        FixedArray<AnimationChannelReference>               animChannelRefs_;
        
        FixedArray<const char*>                             animKeyframeNames_;
        FixedArray<int32_t>                                 animKeyframeIndices_;
        FixedArray<AnimationKeyframeReference>              animKeyframeRefs_;

        FixedArray<AnimationKeyframeReference *>            animChannelKeyframes_;
        
        FixedArray<const char *>                            placementNames_;
        FixedArray<int32_t>                                 placementIndices_;
        FixedArray<PlacementReference>                      placementRefs_;
        
        FixedArray<const char *>                            maskNames_;
        FixedArray<int32_t>                                 maskIndices_;
        FixedArray<MaskReference>                           maskRefs_;
        
        FixedArray<const char *>                            textureMaskNames_;
        FixedArray<int32_t>                                 textureMaskIndices_;
        FixedArray<TextureMaskReference>                    textureMaskRefs_;
        
        FixedArray<ButtonStateReference>                    buttonStateRefs_;

        FixedArray<const char *>                            buttonNames_;
        FixedArray<int32_t>                                 buttonIndices_;
        FixedArray<ButtonReference>                         buttonRefs_;

        FixedArray<const char *>                            externalPackageNames_;
        FixedArray<int32_t>                                 externalPackageIndices_;
        FixedArray<ExternalPackageReference>                externalPackageRefs_;

        FixedArray<AutoDisplayElementReference>             autoDisplayElementRefs_;

        std::unordered_map<std::string, GfxReferenceType>   referenceTypes_;
        
        std::vector<TextureHandle>                          loadedTextures_;
        std::vector<TextureAtlasHandle>                     loadedTextureAtlases_;
        std::vector<FontHandle>                             loadedFonts_;
        
        void loadTextures(std::function<void()> callback);
        void loadFonts(std::function<void()> callback);
        
        void sortChannels(AnimationChannelReferenceIntermediate *channels, int32_t size);
        
        GfxReferenceType referenceTypeForString(std::string type);
        void computeMemoryUsage();
    };
    
    class GameObject;
    
#if TARGET_OS_IPHONE

    typedef void (*ObjCGameObjectCreatedHandler)(__weak id, SEL, GameObject *);

#endif /* TARGET_OS_IPHONE */
    
    using GameObjectCreatedHandler = std::function<void(GameObject *)>;
    
    struct SceneObjectCreatedDelegate {
        // ANY MODIFICATION OF THIS WILL REQUIRE CHANGES TO Space.cpp
        GameObjectCreatedHandler animationSequenceCreatedHandler;
        GameObjectCreatedHandler buttonCreatedHandler;
        GameObjectCreatedHandler maskCreatedHandler;
        GameObjectCreatedHandler placementCreatedHandler;
        GameObjectCreatedHandler spriteCreatedHandler;
        GameObjectCreatedHandler textCreatedHandler;
        GameObjectCreatedHandler textureMaskCreatedHandler;
        // ANY MODIFICATION OF THIS WILL REQUIRE CHANGES TO Space.cpp
        
        void clear() {
            animationSequenceCreatedHandler  = nullptr;
            buttonCreatedHandler  = nullptr;
            maskCreatedHandler  = nullptr;
            placementCreatedHandler  = nullptr;
            spriteCreatedHandler  = nullptr;
            textCreatedHandler  = nullptr;
            textureMaskCreatedHandler  = nullptr;
        }
    };
}

#endif /* ScenePackage_h */
