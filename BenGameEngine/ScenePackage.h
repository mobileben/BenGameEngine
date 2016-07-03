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

namespace BGE {
    struct ScenePackageTag {};
    typedef Handle<ScenePackageTag> ScenePackageHandle;
    
    class ScenePackage : public NamedObject {
    public:
        ScenePackage(ObjectId sceneId);
        ~ScenePackage();
        
        void initialize(ScenePackageHandle handle, ObjectId scenePackageId, std::string name);
        
        void link();
        
        bool hasExternalReferences() const {
            return hasExternal_;
        }
        
        TextureReference *getTextureReference(std::string name);
        TextReference *getTextReference(std::string name);
        AnimationSequenceReference *getAnimationSequenceReference(std::string name);
        
    protected:
        void reset();
        void load(NSDictionary *jsonDict, std::function<void(ScenePackage *)> callback);

    private:
        friend class ScenePackageService;
        
        ScenePackageHandle  handle_;
        std::string         source_;
        float               frameRate_;
        float               width_;
        float               height_;
        Vector2             position_;
        bool                fontsLoaded_;
        bool                texturesLoaded_;
        bool                hasExternal_;
        
        int32_t             defaultPositionIndex_;
        int32_t             defaultScaleIndex_;
        
        FixedArray<char>                                    strings_;
        FixedArray<TextureReferenceIntermediate>            textures_;
        FixedArray<TextReferenceIntermediate>               text_;
        FixedArray<AnimationSequenceReferenceIntermediate>  animationSequences_;
        
        FixedArray<Rect>                                    rects_;
        FixedArray<ColorTransform>                          colorTransforms_;
        FixedArray<ColorMatrix>                             colorMatrices_;
        FixedArray<Vector2>                                 vector2s_;
        
        FixedArray<BoundsReferenceIntermediate>             bounds_;
        FixedArray<AnimationKeyframeReferenceIntermediate>  keyframes_;
        FixedArray<AnimationChannelReferenceIntermediate>   channels_;
        
        std::shared_ptr<std::atomic_int>                    textureCount_;
        std::vector<std::pair<std::string, std::string>>    textureQueue_;
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
        
        void loadTextures(std::function<void()> callback);
        void loadFonts(std::function<void()> callback);
    };
}

#endif /* ScenePackage_h */
