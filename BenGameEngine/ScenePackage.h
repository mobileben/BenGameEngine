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

namespace BGE {
    struct ScenePackageTag {};
    typedef Handle<ScenePackageTag> ScenePackageHandle;
    
    class ScenePackage : public Object {
    public:
        ScenePackage(uint64_t sceneId);
        ~ScenePackage();
        
    protected:
        void reset();
        void load(NSDictionary *jsonDict);

    private:
        friend class ScenePackageService;
        
        std::string source_;
        float frameRate_;
        float width_;
        float height_;
        Vector2 position_;
        FixedArray<char>                strings_;
        FixedArray<TextureReference>    textures_;
//        std::vector<TextureReference>   textures_;
        FixedArray<TextReference> text_;
//        std::vector<TextReference>      text_;
        FixedArray<AnimationSequenceReference> animationSequences_;
//        std::vector<AnimationSequenceReference> animationSequences_;
    };
}

#endif /* ScenePackage_h */
