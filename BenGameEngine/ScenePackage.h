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
#include "Object.h"
#include "Handle.h"
#include "GraphicFormats.h"
#include "FixedArray.h"
#include "MathTypes.h"

namespace BGE {
    struct ScenePackageTag {};
    typedef Handle<ScenePackageTag> ScenePackageHandle;
    
    class ScenePackage : public Object {
    public:
        ScenePackage(uint64_t sceneId);
        ~ScenePackage();
        
        void link();
        
    protected:
        void reset();
        void prelink();
        void load(NSDictionary *jsonDict);

    private:
        friend class ScenePackageService;
        
        std::string source_;
        float frameRate_;
        float width_;
        float height_;
        Vector2 position_;
        FixedArray<char>                                    strings_;
        FixedArray<TextureReferenceIntermediate>            textures_;
        FixedArray<TextReferenceIntermediate>               text_;
        FixedArray<AnimationSequenceReferenceIntermediate>  animationSequences_;
        
        FixedArray<Rect>                                    rects_;
        FixedArray<ColorTransform>                          colorTransforms_;
        FixedArray<ColorMatrix>                             colorMatrices_;
        FixedArray<Vector2>                                 vector2s_;
        
        FixedArray<BoundsReferenceIntermediate>             boundRefs_;
        FixedArray<AnimationKeyFrameReferenceIntermediate>  keyframes_;
        FixedArray<AnimationChannelReferenceIntermediate>   channels_;
        
        std::vector<std::pair<std::string, std::string>>    textureQueue_;
        std::vector<std::pair<std::string, uint32_t>>       fontQueue_;
    };
}

#endif /* ScenePackage_h */
