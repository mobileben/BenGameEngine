//
//  ScenePackage.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ScenePackage_h
#define ScenePackage_h

#include <stdio.h>
#import <Foundation/Foundation.h>
#include <vector>
#include <atomic>
#include <functional>
#include "NamedObject.h"
#include "Handle.h"
#include "GraphicFormats.h"
#include "FixedArray.h"
#include "MathTypes.h"
#include "FileUtilities.h"

namespace BGE {
    enum class ScenePackageStatus {
        Invalid,
        Loading,
        Valid,
        Linked
    };
    
    enum class ScenePackageError : int32_t {
        UnsupportedFormat = 0,
        DoesNotExist,
        Loading,
        NoAvailableHandles
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
        
    protected:
        void setStatus(ScenePackageStatus status) { status_ = status; }
        void create(NSDictionary *jsonDict, std::function<void(ScenePackage *)> callback);

    private:
        friend class ScenePackageService;
        
        struct TextureQueueItem {
            std::string     name;
            FilePath        filePath;
            TextureFormat   format;
        };
        
        ScenePackageStatus                                  status_;
        ScenePackageHandle                                  handle_;
        
        BaseDirectory                                       baseDirectory_;
        
        std::string                                         source_;
        float                                               frameRate_;
        float                                               pixelsPerPoint_;
        float                                               width_;
        float                                               height_;
        Vector2                                             position_;
        bool                                                fontsLoaded_;
        bool                                                texturesLoaded_;
        bool                                                hasExternal_;
        
        int32_t                                             defaultPositionIndex_;
        int32_t                                             defaultScaleIndex_;
        int32_t                                             defaultSkewIndex_;
        int32_t                                             defaultCollisionRectScaleIndex_;
        
        size_t                                              memoryUsage_;

        FixedArray<char>                                    strings_;
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
        
        FixedArray<BoundsReferenceIntermediate>             bounds_;
        FixedArray<AnimationKeyframeReferenceIntermediate>  keyframes_;
        FixedArray<AnimationChannelReferenceIntermediate>   channels_;
        
        std::unordered_map<std::string, std::vector<SubTextureDef>> subTextures_;
        
        std::shared_ptr<std::atomic_int>                    textureCount_;
        std::vector<TextureQueueItem>                       textureQueue_;
        std::shared_ptr<std::atomic_int>                    fontCount_;
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
