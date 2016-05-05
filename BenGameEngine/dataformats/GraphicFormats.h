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
#include "BGEMathTypes.h"

namespace BGE {
    struct AnimationKeyFrameFormat {
        uint32_t totalFrames;
        BGEMatrix3 matrix;
        BGEColorMatrix colorMatrix;
        uint32_t flags;
        BGERect bounds;
    };
    
    struct AnimationChannelFormat {
        char *reference;
        char *referenceType;
        uint32_t totalFrames;
        AnimationKeyFrameFormat *keyframes;
    };
    
    struct AnimationFormat {
        char *name;
        uint32_t fps;
        uint32_t totalFrames;
        uint32_t numChannels;
        AnimationChannelFormat *channels;
    };
    
    struct MaskFormat {
        char *name = nullptr;
        float width = 0;
        float height = 0;
    };
    
    struct ExternalReferenceFormat {
        char *name;
        char *externalPackage;
    };
    
    struct AutoDisplayListElementFormat {
        char *name;
        char *referenceType;
        uint32_t flags;
        uint32_t hidden;
        BGERect bounds;
        BGEVector2 xy;
        BGEVector2 scale;
        float rotation;
        char *reference;
    };
    
    struct PlacementFormat {
        char *name;
        float width;
        float height;
    };
    
    struct TextFormat {
        enum Alignment {
            Left = 0,
            Center = 1,
            Right = 2
        };
        
        char *name = nullptr;
        char *text = nullptr;
        char *fontName = nullptr;
        Alignment alignment;
        uint32_t color = 0;
        float size = 0;
        float leading = 0;
        
        float width = 0;
        float height = 0;
    };
    
    struct SubTextureFormat {
        char *name;
        char *atlas;
        float rotation;
        float x;
        float y;
        float width;
        float height;
        
    };
    
    struct TextureFormat {
        char    *name = nullptr;
        char    *filename = nullptr;
        float   width = 0;
        float   height = 0;
        TextureFormat(char *name, char *filename, float width, float height) : name(name), filename(filename), width(width), height(height) {
        }
    };

    struct ScenePackageFormat {
        char *name;
        
        char ** strings;
    };

}

#endif /* GraphicFormats_h */
