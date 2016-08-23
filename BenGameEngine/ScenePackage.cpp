 //
//  ScenePackage.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/17/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ScenePackage.h"
#include "Game.h"
#include "ArrayBuilder.h"
#include "UniqueArrayBuilder.h"
#include "StringArrayBuilder.h"
#include <type_traits>

BGE::ScenePackage::ScenePackage() : NamedObject(), frameRate_(0), width_(0), height_(0), fontsLoaded_(false), texturesLoaded_(false), hasExternal_(false), defaultPositionIndex_(NullPtrIndex), defaultScaleIndex_(NullPtrIndex), fontCount_(nullptr) {
    position_ = Vector2{0, 0};
    textureCount_ = std::make_shared<std::atomic_int>(0);
    fontCount_ = std::make_shared<std::atomic_int>(0);
}

BGE::ScenePackage::ScenePackage(ObjectId sceneId) : NamedObject(sceneId), frameRate_(0), width_(0), height_(0), fontsLoaded_(false), texturesLoaded_(false), hasExternal_(false), defaultPositionIndex_(NullPtrIndex), defaultScaleIndex_(NullPtrIndex), fontCount_(nullptr) {
    position_ = Vector2{0, 0};
    textureCount_ = std::make_shared<std::atomic_int>(0);
    fontCount_ = std::make_shared<std::atomic_int>(0);
}

BGE::ScenePackage::~ScenePackage() {
    reset();
}

void BGE::ScenePackage::reset() {
    // Release all textures
    
    // Release all fonts
    
    
    textures_.clear();
    text_.clear();
    animationSequences_.clear();
}

void BGE::ScenePackage::initialize(ScenePackageHandle handle, std::string name) {
    handle_ = handle;
    setName(name);
}

void BGE::ScenePackage::destroy() {
}

BGE::AutoDisplayElementReference *BGE::ScenePackage::getAutoDisplayList() const {
    return autoDisplayElementRefs_.baseAddress();
}

int32_t BGE::ScenePackage::getAutoDisplayListSize() const {
    return autoDisplayElementRefs_.getSize();
}

void BGE::ScenePackage::link() {
    // Link Texture
    std::shared_ptr<TextureService> textureService = Game::getInstance()->getTextureService();
    textureNames_ = FixedArray<const char*>(textures_.size());
    textureIndices_ = FixedArray<int32_t>(textures_.size());
    textureRefs_ = FixedArray<TextureReference>(textures_.size());
    
    referenceTypes_.clear();
    
    for (auto i=0;i<textures_.size();i++) {
        TextureReferenceIntermediate *texRefInt = textures_.addressOf(i);
        const char**name = textureNames_.addressOf(i);
        int32_t *index = textureIndices_.addressOf(i);
        TextureReference *texRef = textureRefs_.addressOf(i);
        
        // Fix names
        const char* resolvedName = strings_.addressOf(texRefInt->name);
        const char *resolvedAtlasName = strings_.safeAddressOf(texRefInt->atlasName);
        *name = resolvedName;
        *index = i;
        texRef->name = resolvedName;
        
        if (resolvedAtlasName) {
            // textureHandle is derived from atlas
            auto atlasHandle = textureService->getTextureAtlasHandle(getHandle(), resolvedAtlasName);
            auto subTexHandle = textureService->getTextureHandle(atlasHandle, resolvedName);
            
            texRef->textureHandle = subTexHandle;

            texRef->isAtlas = false;
            texRef->isSubTexture = true;
        } else if (texRefInt->isAtlas) {
            // textureHandle is derived from atlas
            auto atlas = textureService->getTextureAtlas(getHandle(), resolvedName);
            
            texRef->textureHandle = atlas->getTextureHandle();

            texRef->isAtlas = true;
            texRef->isSubTexture = false;
        } else {
            texRef->textureHandle = textureService->getTextureHandle(getHandle(), resolvedName);

            texRef->isAtlas = false;
            texRef->isSubTexture = false;
        }
        
        referenceTypes_[resolvedName] = GfxReferenceTypeSprite;
    }
    
    // Link TextReference
    std::shared_ptr<FontService> fontService = Game::getInstance()->getFontService();
    textNames_ = FixedArray<const char*>(text_.size());
    textIndices_ = FixedArray<int32_t>(text_.size());
    textRefs_ = FixedArray<TextReference>(text_.size());
    
    for (auto i=0;i<text_.size();i++) {
        TextReferenceIntermediate *textRefInt = text_.addressOf(i);
        const char**name = textNames_.addressOf(i);
        int32_t *index = textIndices_.addressOf(i);
        TextReference *textRef = textRefs_.addressOf(i);
        
        // Fix names
        const char* resolvedName = textRefInt->name + strings_.baseAddress();
        const char* resolvedText = textRefInt->text + strings_.baseAddress();
        const char* fontName = textRefInt->fontName + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        textRef->name = resolvedName;
        textRef->text = resolvedText;
        textRef->width = textRefInt->width;
        textRef->height = textRefInt->height;
        textRef->leading = textRefInt->leading;
        textRef->color = textRefInt->color;
        textRef->alignment = textRefInt->alignment;
        
        textRef->fontHandle = fontService->getFontHandle(fontName, textRefInt->size);
        referenceTypes_[resolvedName] = GfxReferenceTypeText;
    }
    
    // Link BoundsRefs
    boundsRefs_ = FixedArray<BoundsReference>(bounds_.size());
    
    for (auto i=0;i<bounds_.size();i++) {
        BoundsReferenceIntermediate *boundsRefInt = bounds_.addressOf(i);
        BoundsReference *boundsRef = boundsRefs_.addressOf(i);
        
        Rect *rect = boundsRefInt->bounds + rects_.baseAddress();
        boundsRef->startFrame = boundsRefInt->startFrame;
        boundsRef->totalFrames = boundsRefInt->totalFrames;
        boundsRef->bounds = rect;

    }
    
    // Link AnimationKeyframeReference
    animKeyframeRefs_ = FixedArray<AnimationKeyframeReference>(keyframes_.size());
    
    for (auto i=0;i<keyframes_.size();i++) {
        AnimationKeyframeReferenceIntermediate *keyframeRefInt = keyframes_.addressOf(i);
        AnimationKeyframeReference *keyframeRef = animKeyframeRefs_.addressOf(i);
        
        keyframeRef->startFrame = keyframeRefInt->startFrame;
        keyframeRef->totalFrames = keyframeRefInt->totalFrames;
        keyframeRef->order = keyframeRefInt->order;
        keyframeRef->flags = keyframeRefInt->flags;
        keyframeRef->frame = keyframeRefInt->frame;
        if (keyframeRefInt->position != NullPtrIndex) {
            keyframeRef->position = keyframeRefInt->position + vector2s_.baseAddress();
        } else {
            keyframeRef->position = nullptr;
        }
        if (keyframeRefInt->scale != NullPtrIndex) {
            keyframeRef->scale = keyframeRefInt->scale + vector2s_.baseAddress();
        } else {
            keyframeRef->scale = nullptr;
        }
        keyframeRef->rotation = keyframeRefInt->rotation;
        keyframeRef->matrix = nullptr;  // TODO
        if (keyframeRefInt->colorMatrix != NullPtrIndex) {
            keyframeRef->colorMatrix = keyframeRefInt->colorMatrix + colorMatrices_.baseAddress();
        } else {
            keyframeRef->colorMatrix = nullptr;
        }
        if (keyframeRefInt->colorTransform != NullPtrIndex) {
            keyframeRef->colorTransform = keyframeRefInt->colorTransform + colorTransforms_.baseAddress();
        } else {
            keyframeRef->colorTransform = nullptr;
        }
        if (keyframeRefInt->bounds != NullPtrIndex) {
            keyframeRef->bounds = keyframeRefInt->bounds + rects_.baseAddress();
        } else {
            keyframeRef->bounds = nullptr;
        }
    }

    // Build FixedArray for keyframe pointers
    animChannelKeyframes_ = FixedArray<AnimationKeyframeReference *>(keyframes_.size());

    // Link AnimationChannelReference
    animChannelRefs_ = FixedArray<AnimationChannelReference>(channels_.size());
    int32_t keyframeIndex = 0;
    
    for (auto i=0;i<channels_.size();i++) {
        AnimationChannelReferenceIntermediate *animChannelIntRef = channels_.addressOf(i);
        AnimationChannelReference *animChannelRef = animChannelRefs_.addressOf(i);

        // Fix names
        const char* resolvedName = animChannelIntRef->name + strings_.baseAddress();
        const char* reference = animChannelIntRef->reference + strings_.baseAddress();

        animChannelRef->name = resolvedName;
        animChannelRef->reference = reference;
        animChannelRef->referenceType = animChannelIntRef->referenceType;
         // Build the links related to keyframes
        animChannelRef->numKeyframes = (int32_t) animChannelIntRef->keyframes.size();
        animChannelRef->keyframes = animChannelKeyframes_.addressOf(keyframeIndex);
        
        for (auto ki=0;ki<animChannelRef->numKeyframes;ki++) {
            animChannelRef->keyframes[ki] = animKeyframeRefs_.addressOf(animChannelIntRef->keyframes[ki]);
        }
        
        keyframeIndex += animChannelRef->numKeyframes;
    }
    
    // Link AnimationSequenceReference
    animSeqNames_ = FixedArray<const char*>(animationSequences_.size());
    animSeqIndices_ = FixedArray<int32_t>(animationSequences_.size());
    animSeqRefs_ = FixedArray<AnimationSequenceReference>(animationSequences_.size());

    for (auto i=0;i<animationSequences_.size();i++) {
        AnimationSequenceReferenceIntermediate *animSeqRefInt = animationSequences_.addressOf(i);
        AnimationSequenceReference *animSeqRef = animSeqRefs_.addressOf(i);
        const char**name = animSeqNames_.addressOf(i);
        int32_t *index = animSeqIndices_.addressOf(i);

        // Fix names
        const char* resolvedName = animSeqRefInt->name + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        animSeqRef->name = resolvedName;
        animSeqRef->frameRate = animSeqRefInt->frameRate;
        animSeqRef->totalFrames = animSeqRefInt->totalFrames;
        animSeqRef->numChannels = animSeqRefInt->numChannels;
        animSeqRef->numBounds = animSeqRefInt->numBounds;
        
        animSeqRef->channels = animChannelRefs_.addressOf(animSeqRefInt->channels);
        animSeqRef->bounds = boundsRefs_.addressOf(animSeqRefInt->bounds);
        referenceTypes_[resolvedName] = GfxReferenceTypeAnimationSequence;
    }
    
    // Link Placements
    placementNames_ = FixedArray<const char*>(placements_.size());
    placementIndices_ = FixedArray<int32_t>(placements_.size());
    placementRefs_ = FixedArray<PlacementReference>(placements_.size());

    for (auto i=0;i<placements_.size();i++) {
        PlacementIntermediate *placementInt = placements_.addressOf(i);
        PlacementReference *placement = placementRefs_.addressOf(i);
        const char**name = placementNames_.addressOf(i);
        int32_t *index = placementIndices_.addressOf(i);
        
        // Fix names
        const char* resolvedName = placementInt->name + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        placement->name = resolvedName;
        placement->width = placementInt->width;
        placement->height = placementInt->height;
        referenceTypes_[resolvedName] = GfxReferenceTypePlacement;
    }
    
    // Link Masks
    maskNames_ = FixedArray<const char*>(masks_.size());
    maskIndices_ = FixedArray<int32_t>(masks_.size());
    maskRefs_ = FixedArray<MaskReference>(masks_.size());
    
    for (auto i=0;i<masks_.size();i++) {
        MaskIntermediate *maskInt = masks_.addressOf(i);
        MaskReference *mask = maskRefs_.addressOf(i);
        const char**name = maskNames_.addressOf(i);
        int32_t *index = maskIndices_.addressOf(i);
        
        // Fix names
        const char* resolvedName = maskInt->name + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        mask->name = resolvedName;
        mask->width = maskInt->width;
        mask->height = maskInt->height;
        referenceTypes_[resolvedName] = GfxReferenceTypeMask;
    }
    
    // Link Texture Masks
    textureMaskNames_ = FixedArray<const char*>(textureMasks_.size());
    textureMaskIndices_ = FixedArray<int32_t>(textureMasks_.size());
    textureMaskRefs_ = FixedArray<TextureMaskReference>(textureMasks_.size());
    
    for (auto i=0;i<textureMasks_.size();i++) {
        TextureMaskIntermediate *maskInt = textureMasks_.addressOf(i);
        TextureMaskReference *mask = textureMaskRefs_.addressOf(i);
        const char**name = textureMaskNames_.addressOf(i);
        int32_t *index = textureMaskIndices_.addressOf(i);
        
        // Fix names
        const char* resolvedName = maskInt->name + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        mask->name = resolvedName;
        mask->texture = getTextureReference(resolvedName);
        referenceTypes_[resolvedName] = GfxReferenceTypeTextureMask;
    }
    
    // Link Button States
    buttonStateRefs_ = FixedArray<ButtonStateReference>(buttonStates_.size());
    
    for (auto i=0;i<buttonStates_.size();i++) {
        ButtonStateIntermediate *buttonStateInt = buttonStates_.addressOf(i);
        ButtonStateReference *buttonState = buttonStateRefs_.addressOf(i);
        const char *resolvedState = buttonStateInt->state + strings_.baseAddress();
        const char *resolvedReference = buttonStateInt->reference + strings_.baseAddress();
        
        buttonState->state = resolvedState;
        buttonState->reference = resolvedReference;
        buttonState->referenceType = buttonStateInt->referenceType;
    }
    
    // Link Buttons
    buttonNames_ = FixedArray<const char*>(buttons_.size());
    buttonIndices_ = FixedArray<int32_t>(buttons_.size());
    buttonRefs_ = FixedArray<ButtonReference>(buttons_.size());
    
    for (auto i=0;i<buttons_.size();i++) {
        ButtonIntermediate *buttonInt = buttons_.addressOf(i);
        ButtonReference *button = buttonRefs_.addressOf(i);
        const char**name = buttonNames_.addressOf(i);
        int32_t *index = buttonIndices_.addressOf(i);
        
        // Fix names
        const char* resolvedName = buttonInt->name + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        button->name = resolvedName;
        button->states = buttonStateRefs_.addressOf(buttonInt->states);
        button->numStates = buttonInt->numStates;
        referenceTypes_[resolvedName] = GfxReferenceTypeButton;
    }
    
    // Link External References
    externalPackageNames_ = FixedArray<const char*>(externalPackages_.size());
    externalPackageIndices_ = FixedArray<int32_t>(externalPackages_.size());
    externalPackageRefs_ = FixedArray<ExternalPackageReference>(externalPackages_.size());
    
    for (auto i=0;i<externalPackages_.size();i++) {
        ExternalPackageIntermediate *externalPackageInt = externalPackages_.addressOf(i);
        ExternalPackageReference *externalPackage = externalPackageRefs_.addressOf(i);
        const char**name = externalPackageNames_.addressOf(i);
        int32_t *index = externalPackageIndices_.addressOf(i);
        
        // Fix names
        const char* resolvedName = externalPackageInt->name + strings_.baseAddress();
        const char *resolvedExternal = externalPackageInt->externalPackage + strings_.baseAddress();
        
        *name = resolvedName;
        *index = i;
        externalPackage->name = resolvedName;
        externalPackage->externalPackage = resolvedExternal;
        referenceTypes_[resolvedName] = GfxReferenceTypeExternalReference;
    }
    
    // Link Auto Display Elements
    autoDisplayElementRefs_ = FixedArray<AutoDisplayElementReference>(autoDisplayElements_.size());
    
    for (auto i=0;i<autoDisplayElements_.size();i++) {
        AutoDisplayElementIntermediate *elemInt = autoDisplayElements_.addressOf(i);
        AutoDisplayElementReference *elem = autoDisplayElementRefs_.addressOf(i);
        const char *resolvedName = elemInt->name + strings_.baseAddress();
        const char *referenceResolvedName = elemInt->reference + strings_.baseAddress();
        
        elem->name = resolvedName;
        elem->reference = referenceResolvedName;
        elem->referenceType = elemInt->referenceType;
        elem->flags = elemInt->flags;
        elem->hidden = elemInt->hidden;
        
        if (elemInt->position != NullPtrIndex) {
            elem->position = elemInt->position + vector2s_.baseAddress();
        } else {
            elem->position = nullptr;
        }
        if (elemInt->scale != NullPtrIndex) {
            elem->scale = elemInt->scale + vector2s_.baseAddress();
        } else {
            elem->scale = nullptr;
        }
        elem->rotation = elemInt->rotation;
        elem->matrix = nullptr;  // TODO default matrix and default color matrix and default color transform
        if (elemInt->colorMatrix != NullPtrIndex) {
            elem->colorMatrix = elemInt->colorMatrix + colorMatrices_.baseAddress();
        } else {
            elem->colorMatrix = nullptr;
        }
        if (elemInt->colorTransform != NullPtrIndex) {
            elem->colorTransform = elemInt->colorTransform + colorTransforms_.baseAddress();
        } else {
            elem->colorTransform = nullptr;
        }
        if (elemInt->bounds != NullPtrIndex) {
            elem->bounds = elemInt->bounds + rects_.baseAddress();
        } else {
            elem->bounds = nullptr;
        }
    }
}

void BGE::ScenePackage::load(NSDictionary *jsonDict, std::function<void(ScenePackage *)> callback) {
    reset();
    float platformScale = 2.0;
    
    // Textures
    NSArray *subtextures = jsonDict[@"subtextures"];
    NSArray *textures = jsonDict[@"textures"];
    StringArrayBuilder stringBuilder ;
    ArrayBuilder<TextureReferenceIntermediate, TextureReferenceIntermediate> texIntBuilder;
    ArrayBuilder<TextReferenceIntermediate, TextReferenceIntermediate> textIntBuilder;
    ArrayBuilder<AnimationSequenceReferenceIntermediate, AnimationSequenceReferenceIntermediate> animSeqIntBuilder;
    ArrayBuilder<PlacementIntermediate, PlacementIntermediate> placementIntBuilder;
    ArrayBuilder<MaskIntermediate, MaskIntermediate> maskIntBuilder;
    ArrayBuilder<TextureMaskIntermediate, TextureMaskIntermediate> textureMaskIntBuilder;
    ArrayBuilder<ButtonStateIntermediate, ButtonStateIntermediate> buttonStateIntBuilder;
    ArrayBuilder<ButtonIntermediate, ButtonIntermediate> buttonIntBuilder;
    ArrayBuilder<ExternalPackageIntermediate, ExternalPackageIntermediate> extPackageIntBuilder;
    ArrayBuilder<AutoDisplayElementIntermediate, AutoDisplayElementIntermediate> autoDisplayElemIntBuilder;
    
    UniqueArrayBuilder<Rect, Rect> rectBuilder;
    UniqueArrayBuilder<ColorTransform, ColorTransform> colorXformBuilder;
    UniqueArrayBuilder<ColorMatrix, ColorMatrix> colorMtxBuilder;
    UniqueArrayBuilder<Vector2, Vector2> vector2Builder;

    ArrayBuilder<BoundsReferenceIntermediate, BoundsReferenceIntermediate> boundsRefIntBuilder;
    ArrayBuilder<AnimationKeyframeReferenceIntermediate, AnimationKeyframeReferenceIntermediate> keyframeIntBuilder;
    ArrayBuilder<AnimationChannelReferenceIntermediate, AnimationChannelReferenceIntermediate> channelRefIntBuilder;
    
    width_ = [jsonDict[@"width"] floatValue] * platformScale;
    height_ = [jsonDict[@"height"] floatValue] * platformScale;
    source_ = [jsonDict[@"source"] UTF8String];
    frameRate_ = [jsonDict[@"frameRate"] floatValue];
    position_.x = [jsonDict[@"position"][@"x"] floatValue] * platformScale;
    position_.y = [jsonDict[@"position"][@"y"] floatValue] * platformScale;
    
    defaultPositionIndex_ = vector2Builder.add(Vector2{0, 0});
    defaultScaleIndex_ = vector2Builder.add(Vector2{1, 1});

    // TextureReferenceIntermediate total count are all textures and subTextures
    texIntBuilder.resize((int32_t) (textures.count + subtextures.count));
    
    // Do subtextures first, since we will use this info to know if our textures are atlases
    
    // Subtextures
    for (auto index=0;index<subtextures.count;index++) {
        NSDictionary *subTexDict = subtextures[index];
        TextureReferenceIntermediate *texRef = texIntBuilder.addressOf(index);

        SubTextureDef subTexDef;
        
        subTexDef.name = [subTexDict[@"name"] UTF8String];
        subTexDef.x = [subTexDict[@"x"] floatValue] * platformScale;
        subTexDef.y = [subTexDict[@"y"] floatValue] * platformScale;
        subTexDef.width = [subTexDict[@"width"] floatValue] * platformScale;
        subTexDef.height = [subTexDict[@"height"] floatValue] * platformScale;
        subTexDef.rotated = [subTexDict[@"rotated"] boolValue];
        
        NSString *atlasName = subTexDict[@"atlas"];
        std::vector<SubTextureDef> &subTexDefs = subTextures_[[atlasName UTF8String]];
        
        subTexDefs.push_back(subTexDef);
        
        texRef->name = stringBuilder.add(subTexDef.name.c_str());
        texRef->atlasName = stringBuilder.add([atlasName UTF8String]);
        texRef->width = subTexDef.width;
        texRef->height = subTexDef.height;
        texRef->isAtlas = false;
    }
    
    for (int32_t index=0;index<textures.count;index++) {
        NSDictionary *texDict = textures[index];
        TextureReferenceIntermediate *texRef = texIntBuilder.addressOf((int) (subtextures.count + index));
        
        // Add name
        const char* name = [texDict[@"name"] UTF8String];
        
        assert(name);
        
        if (name) {
            texRef->name = stringBuilder.add(name);
            texRef->atlasName = NullPtrIndex;
            texRef->width = [texDict[@"width"] floatValue] * platformScale;
            texRef->height = [texDict[@"height"] floatValue] * platformScale;
            
            auto subTex = subTextures_.find(name);
            
            if (subTex != subTextures_.end()) {
                texRef->isAtlas = true;
            } else {
                texRef->isAtlas = false;
            }
            
            // Now get the file information
            NSString* filename = [[NSBundle mainBundle] pathForResource:texDict[@"filename"] ofType:nil];
            
            if (filename) {
                textureQueue_.push_back(std::make_pair<std::string, std::string>(name, [filename UTF8String]));
            }
        }
    }

    // Text
    NSArray *text = jsonDict[@"text"];
    
    textIntBuilder.resize((int32_t) text.count);
    
    for (int32_t index=0;index<text.count;index++) {
        NSDictionary *textDict = text[index];
        TextReferenceIntermediate *textRef = textIntBuilder.addressOf(index);
        
        const char *name = [textDict[@"name"] UTF8String];
        const char *text = [textDict[@"text"] UTF8String];
        
        assert(name);
        
        if (name) {
            NSDictionary *formatDict = textDict[@"format"];
            
            textRef->name = stringBuilder.add(name);
            textRef->text = stringBuilder.add(text);
            textRef->width = [textDict[@"width"] floatValue] * platformScale;
            textRef->height = [textDict[@"height"] floatValue] * platformScale;
            textRef->leading = [formatDict[@"leading"] floatValue] * platformScale;
            
            std::string alignment = [formatDict[@"alignment"] UTF8String];
            
            if (alignment == "left"){
                textRef->alignment = FontHorizontalAlignment::Left;
            } else if (alignment == "right"){
                textRef->alignment = FontHorizontalAlignment::Right;
            } else {
                textRef->alignment = FontHorizontalAlignment::Center;
            }
            
            uint32_t size = [formatDict[@"fontSize"] unsignedIntValue] * 2;
            uint32_t color = [formatDict[@"fontColor"] unsignedIntValue];
            uint32_t blue = color & 0xff;
            uint32_t green = color >> 8 & 0xff;
            uint32_t red = color >> 16 & 0xff;
            uint32_t alpha = 255;
            
            textRef->size = size;
            textRef->color.r = red / 255.;
            textRef->color.g = green / 255.;
            textRef->color.b = blue / 255.;
            textRef->color.a = alpha / 255.;
            
            const char* fontName = [formatDict[@"fontName"] UTF8String];
            textRef->fontName = stringBuilder.add(fontName);
            
            // Make sure
            bool found = false;
            
            for (auto f : fontQueue_) {
                if (f.first == fontName && f.second == size) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                fontQueue_.push_back(std::make_pair<std::string, int32_t>(fontName, size));
            }
            
        }
    }
    
    // Symbols
    NSArray *symbols = jsonDict[@"symbols"];
    
    animSeqIntBuilder.resize((int32_t) symbols.count);
    
    for (int32_t index=0;index<symbols.count;index++) {
        NSDictionary *symbolDict = symbols[index];
        AnimationSequenceReferenceIntermediate *animSeq = animSeqIntBuilder.addressOf(index);
        
        const char *name = [symbolDict[@"name"] UTF8String];
        
        if (name) {
            animSeq->name = stringBuilder.add(name);
            animSeq->frameRate = frameRate_;
            animSeq->bounds = (uint32_t) boundsRefIntBuilder.size();
            animSeq->channels = (uint32_t) channelRefIntBuilder.size();
            
            NSArray *frames = symbolDict[@"frames"];
            std::map<std::string, int32_t> channelIndex;
            
            BoundsReference currBounds;
            BoundsReference newBounds;
            Rect currRect;
            Rect newRect;
            
            currBounds.startFrame = 0;
            currBounds.totalFrames = 1;
        
            animSeq->totalFrames = (uint32_t) frames.count;
            
            for (NSInteger fi=0;fi<frames.count;fi++) {
                NSDictionary *frameDict = frames[fi];
                NSArray *children = frameDict[@"children"];
                
                if (fi == 0) {
                    currRect.x = [frameDict[@"bounds"][@"x"] floatValue] * platformScale;
                    currRect.y = [frameDict[@"bounds"][@"y"] floatValue] * platformScale;
                    currRect.w = [frameDict[@"bounds"][@"width"] floatValue] * platformScale;
                    currRect.h = [frameDict[@"bounds"][@"height"] floatValue] * platformScale;
                } else {
                    newBounds.startFrame = 0;
                    newBounds.totalFrames = 1;
                    newRect.x = [frameDict[@"bounds"][@"x"] floatValue] * platformScale;
                    newRect.y = [frameDict[@"bounds"][@"y"] floatValue] * platformScale;
                    newRect.w = [frameDict[@"bounds"][@"width"] floatValue] * platformScale;
                    newRect.h = [frameDict[@"bounds"][@"height"] floatValue] * platformScale;
                    
                    if (newRect.x != currRect.x || newRect.y != currRect.y || newRect.w != currRect.w || newRect.h != currRect.h) {
                        currRect = newRect;
                        boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currRect)});
                    } else {
                        currBounds.totalFrames++;
                    }
                }
                
                for (NSInteger ci=0;ci<children.count;ci++) {
                    NSDictionary *childDict = children[ci];
                    const char *childName = [childDict[@"name"] UTF8String];
                    
                    assert(childName);

                    if (childName) {
                        auto channelIndexIt = channelIndex.find(childName);
                        AnimationChannelReferenceIntermediate *channel;
                        if (channelIndexIt != channelIndex.end()) {
                            channel = channelRefIntBuilder.addressOf(channelIndexIt->second);
                        } else {
                            AnimationChannelReferenceIntermediate newChannel;
                            const char* reference = [childDict[@"reference"] UTF8String];
                            const char *type = [childDict[@"referenceType"] UTF8String];
                            
                            assert(reference && type);
                            
                            newChannel.name = stringBuilder.add(childName);
                            newChannel.reference = stringBuilder.add(reference);
                            
                            GfxReferenceType value = referenceTypeForString(type);
                            
                            newChannel.referenceType = value;
                            
                            auto idx = channelRefIntBuilder.size();
                            channelIndex[childName] = idx;
                            channelRefIntBuilder.add(newChannel);
                            channel = channelRefIntBuilder.addressOf(idx);
                        }
                        
                        AnimationKeyframeReferenceIntermediate keyframe;
                        Rect boundsBacking;
                        Rect *bounds = &boundsBacking;
                        ColorTransform colorTransformBacking;
                        ColorTransform *colorTransform = &colorTransformBacking;
                        ColorMatrix colorMatrixBacking;
                        ColorMatrix *colorMatrix = &colorMatrixBacking;
                        Vector2 scaleBacking;
                        Vector2 *scale = &scaleBacking;
                        Vector2 positionBacking;
                        Vector2 *position = &positionBacking;

                        keyframe.startFrame = (uint32_t)fi;
                        keyframe.totalFrames = 1;
                        keyframe.order = (uint32_t)ci;
                        keyframe.flags = [childDict[@"flags"] unsignedIntValue];
                        keyframe.frame = [childDict[@"frame"] unsignedIntValue] - 1;
                        
                        // Bounds
                        
                        boundsBacking.x = [childDict[@"bounds"][@"x"] floatValue] * platformScale;
                        boundsBacking.y = [childDict[@"bounds"][@"y"] floatValue] * platformScale;
                        boundsBacking.w = [childDict[@"bounds"][@"width"] floatValue] * platformScale;
                        boundsBacking.h = [childDict[@"bounds"][@"height"] floatValue] * platformScale;
                        
                        // Color
                        if (childDict[@"color"]) {
                            colorTransformBacking.multiplier.r = [childDict[@"color"][@"redMultiplier"] floatValue];
                            colorTransformBacking.multiplier.g = [childDict[@"color"][@"greenMultiplier"] floatValue];
                            colorTransformBacking.multiplier.b = [childDict[@"color"][@"blueMultiplier"] floatValue];
                            colorTransformBacking.multiplier.a = [childDict[@"color"][@"alphaMultiplier"] floatValue];
                            colorTransformBacking.offset.r = [childDict[@"color"][@"redOffset"] floatValue];
                            colorTransformBacking.offset.g = [childDict[@"color"][@"greenOffset"] floatValue];
                            colorTransformBacking.offset.b = [childDict[@"color"][@"blueOffset"] floatValue];
                            colorTransformBacking.offset.a = [childDict[@"color"][@"alphaOffset"] floatValue];
                            colorTransformBacking.color = [childDict[@"color"][@"color"] floatValue];
                        } else {
                            colorTransform = nullptr;
                        }
                        
                        // Filters
                        colorMatrix = nullptr;
                        
                        if (childDict[@"filters"]) {
                            NSArray *filters = childDict[@"filters"];
                            
                            for (NSDictionary *filter in filters) {
                                NSArray *rawMatrix = filter[@"ColorMatrixFilter"][@"matrix"];
                                
                                if (rawMatrix) {
#if 0
                                    for (NSInteger cmi=0;cmi<rawMatrix.count;cmi++) {
                                        colorMatrixBacking.c[cmi] = [rawMatrix[cmi] floatValue];
                                    }
#endif
                                    
                                    float divisor = 1;
                                    float divisor1 = 255.0;
                                    
                                    colorMatrixBacking.matrix.m[0] = [rawMatrix[0] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[4] = [rawMatrix[1] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[8] = [rawMatrix[2] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[12] = [rawMatrix[3] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[1] = [rawMatrix[5] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[5] = [rawMatrix[6] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[9] = [rawMatrix[7] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[13] = [rawMatrix[8] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[2] = [rawMatrix[10] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[6] = [rawMatrix[11] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[10] = [rawMatrix[12] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[14] = [rawMatrix[13] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[3] = [rawMatrix[15] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[7] = [rawMatrix[16] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[11] = [rawMatrix[17] floatValue] / divisor;
                                    colorMatrixBacking.matrix.m[15] = [rawMatrix[18] floatValue] / divisor;
                                    colorMatrixBacking.offset.v[0] = [rawMatrix[4] floatValue] / divisor1;
                                    colorMatrixBacking.offset.v[1] = [rawMatrix[9] floatValue] / divisor1;
                                    colorMatrixBacking.offset.v[2] = [rawMatrix[14] floatValue] / divisor1;
                                    colorMatrixBacking.offset.v[3] = [rawMatrix[19] floatValue] / divisor1;

                                    colorMatrix = &colorMatrixBacking;
                                }
                            }
                        }
                        
                        // Position
                        if (childDict[@"position"]) {
                            
                            positionBacking.x = [childDict[@"position"][@"x"] floatValue] * platformScale;
                            positionBacking.y = [childDict[@"position"][@"y"] floatValue] * platformScale;
                            
                            keyframe.position = (int32_t) vector2Builder.add(positionBacking);
                        } else {
                            position = nullptr;
                        }
                        
                        // Scale
                        if (childDict[@"scale"]) {
                            scaleBacking.x = [childDict[@"scale"][@"x"] floatValue];
                            scaleBacking.y = [childDict[@"scale"][@"y"] floatValue];
                        } else {
                            scale = nullptr;
                        }
                        
                        // Rotation
                        if (childDict[@"rotation"]) {
                            keyframe.rotation = [childDict[@"rotation"] floatValue];
                        } else {
                            keyframe.rotation = 0;
                        }
                        
                        // TODO: Now build matrix

                        // Now determine if this Keyframe is a new Keyframe
                        bool isNew = false;
                        AnimationKeyframeReferenceIntermediate *currKeyframe = nullptr;
                        int32_t keyframeIndex;

                        if (channel->keyframes.size() == 0) {
                            isNew = true;
                        } else {
                            keyframeIndex = channel->keyframes[channel->keyframes.size() - 1];
                            
                            AnimationKeyframeReferenceIntermediate *currKeyframe = &keyframeIntBuilder[keyframeIndex];
                            
                            // Now we need to determine if this is a new Keyframe or existing
                            if (currKeyframe->order != keyframe.order) {
                                isNew = true;
                            } else if (currKeyframe->flags != keyframe.flags) {
                                isNew = true;
                            } else if (currKeyframe->frame != keyframe.frame) {
                                isNew = true;
                            } else {
                                Rect *currBounds;
                                ColorTransform *currColorTransform;
                                ColorMatrix *currColorMatrix;
                                Vector2 *currPosition;
                                Vector2 *currScale;

                                currBounds = rectBuilder.safeAddressOf(currKeyframe->bounds);
                                currColorTransform = colorXformBuilder.safeAddressOf(currKeyframe->colorTransform);
                                currColorMatrix = colorMtxBuilder.safeAddressOf(currKeyframe->colorMatrix);
                                currPosition = vector2Builder.safeAddressOf(currKeyframe->position);
                                currScale = vector2Builder.safeAddressOf(currKeyframe->scale);
                                
                                // Bounds the same?
                                if ((currBounds != bounds) || ((currBounds && bounds) && (*currBounds != *bounds))) {
                                    isNew = true;
                                } else if ((currPosition != position) || ((currPosition && position) && (*currPosition != *position))) {
                                    isNew = true;
                                } else if ((currScale != scale) || ((currScale && scale) && (*currScale != *scale))) {
                                    isNew = true;
                                } else if ((currColorTransform != colorTransform) || ((currColorTransform && colorTransform) && (*currColorTransform != *colorTransform))) {
                                    isNew = true;
                                } else if ((currColorMatrix != colorMatrix) || ((currColorMatrix && colorMatrix) && (*currColorMatrix != *colorMatrix))) {
                                    isNew = true;
                                }
                            }
                        }
                        
                        if (isNew) {
                            // This is a new Keyframe, key values already set, but if needed, we need to assign the offsets
                            keyframe.bounds = rectBuilder.add(boundsBacking);
                            
                            if (position) {
                                keyframe.position = vector2Builder.add(*position);
                            } else {
                                keyframe.position = defaultPositionIndex_;
                            }
                            
                            if (scale) {
                                keyframe.scale = vector2Builder.add(*scale);
                            } else {
                                keyframe.scale = defaultScaleIndex_;
                            }
                            
                            if (colorTransform) {
                                keyframe.colorTransform = colorXformBuilder.add(*colorTransform);
                            } else {
                                keyframe.colorTransform = NullPtrIndex;
                            }
                            
                            if (colorMatrix) {
                                keyframe.colorMatrix = colorMtxBuilder.add(*colorMatrix);
                            } else {
                                keyframe.colorMatrix = NullPtrIndex;
                            }
                            
                            keyframeIndex = keyframeIntBuilder.add(keyframe);

                            channel->keyframes.push_back(keyframeIndex);
                        } else if (currKeyframe) {
                            currKeyframe->totalFrames++;
                        } else {
                            assert(false);
                        }
                    }
                }
            }
           
            boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currRect)});
            
            animSeq->numBounds = (uint32_t) boundsRefIntBuilder.size() - (uint32_t) animSeq->bounds;
            animSeq->numChannels = (uint32_t) channelRefIntBuilder.size() - animSeq->channels;
        }
    }
    
    // Placements
    NSArray *placements = jsonDict[@"placements"];
    
    placementIntBuilder.resize((int32_t) placements.count);
    
    for (int32_t index=0;index<placements.count;index++) {
        NSDictionary *dict = placements[index];
        PlacementIntermediate *placement = placementIntBuilder.addressOf(index);
        const char *name = [dict[@"name"] UTF8String];

        assert(name);
        
        if (name) {
            placement->name = stringBuilder.add(name);
            placement->width = [dict[@"width"] floatValue] * platformScale;
            placement->height = [dict[@"height"] floatValue] * platformScale;
        }
    }
    
    // Buttons
    NSArray *buttons = jsonDict[@"buttons"];
    int32_t numStates = 0;
    
    for (int32_t index=0;index<buttons.count;index++) {
        NSDictionary *buttonDict = buttons[index];
        NSArray *states = buttonDict[@"states"];
        
        numStates += states.count;
    }
    
    buttonIntBuilder.resize((int32_t) buttons.count);
    buttonStateIntBuilder.resize(numStates);
    
    int32_t stateIndex = 0;
    
    for (int32_t index=0;index<buttons.count;index++) {
        NSDictionary *buttonDict = buttons[index];
        ButtonIntermediate *button = buttonIntBuilder.addressOf(index);
        const char *buttonName = [buttonDict[@"name"] UTF8String];
        
        assert(buttonName);
        
        if (buttonName) {
            NSArray *states = buttonDict[@"states"];
            
            button->name = stringBuilder.add(buttonName);
            button->states = stateIndex;
            button->numStates = (int32_t) states.count;
            
            for (NSDictionary *state in states) {
                ButtonStateIntermediate *buttonState = buttonStateIntBuilder.addressOf(stateIndex);
                const char *stateName = [state[@"state"] UTF8String];
                const char *reference = [state[@"reference"] UTF8String];
                const char *type = [state[@"referenceType"] UTF8String];

                assert(stateName);
                assert(reference);
                assert(type);
                
                if (stateName) {
                    GfxReferenceType refType = referenceTypeForString(type);
                    
                    buttonState->state = stringBuilder.add(stateName);
                    buttonState->reference = stringBuilder.add(reference);
                    buttonState->referenceType = refType;
                    
                    stateIndex++;
                }
            }
        }
    }
    
    // Masks
    NSArray *masks = jsonDict[@"masks"];
    
    maskIntBuilder.resize((int32_t) masks.count);
    
    for (int32_t index=0;index<masks.count;index++) {
        NSDictionary *dict = masks[index];
        MaskIntermediate *mask = maskIntBuilder.addressOf(index);
        const char *name = [dict[@"name"] UTF8String];
        
        assert(name);
        
        if (name) {
            mask->name = stringBuilder.add(name);
            mask->width = [dict[@"width"] floatValue] * platformScale;
            mask->height = [dict[@"height"] floatValue] * platformScale;
        }
    }

    // Texture Masks
    NSArray *textureMasks = jsonDict[@"textureMasks"];
    
    textureMaskIntBuilder.resize((int32_t) textureMasks.count);
    
    for (int32_t index=0;index<textureMasks.count;index++) {
        NSDictionary *dict = textureMasks[index];
        TextureMaskIntermediate *mask = textureMaskIntBuilder.addressOf(index);
        const char *name = [dict[@"name"] UTF8String];
        
        assert(name);
        
        if (name) {
            mask->name = stringBuilder.add(name);
        }
    }
    
    // External References
    NSArray *externals = jsonDict[@"externalReferences"];
    
    extPackageIntBuilder.resize((int32_t) externals.count);
    
    for (int32_t index=0;index<externals.count;index++) {
        NSDictionary *dict = externals[index];
        ExternalPackageIntermediate *external = extPackageIntBuilder.addressOf(index);
        const char *name = [dict[@"name"] UTF8String];
        const char *externalPackage = [dict[@"externalPackage"] UTF8String];
        
        assert(name);
        assert(externalPackage);
        
        external->name = stringBuilder.add(name);
        external->externalPackage = stringBuilder.add(externalPackage);
    }
    
    // Auto display list elements
    NSArray *autoDisplayElems = jsonDict[@"autoDisplayList"];
    
    autoDisplayElemIntBuilder.resize((int32_t) autoDisplayElems.count);
    
    for (int32_t index=0;index<autoDisplayElems.count;index++) {
        NSDictionary *dict = autoDisplayElems[index];
        AutoDisplayElementIntermediate *elem = autoDisplayElemIntBuilder.addressOf(index);
        const char *name = [dict[@"name"] UTF8String];
        const char *reference = [dict[@"reference"] UTF8String];
        const char *type = [dict[@"referenceType"] UTF8String];
        
        assert(name);
        assert(reference);
        assert(type);
        
        GfxReferenceType refType = referenceTypeForString(type);
        
        elem->name = stringBuilder.add(name);
        elem->reference = stringBuilder.add(reference);
        elem->referenceType = refType;
        
        Rect bounds;
        
        bounds.x = [dict[@"bounds"][@"x"] floatValue];
        bounds.y = [dict[@"bounds"][@"y"] floatValue];
        bounds.w = [dict[@"bounds"][@"width"] floatValue];
        bounds.h = [dict[@"bounds"][@"height"] floatValue];
        
        elem->bounds = rectBuilder.add(bounds);
        
        if (dict[@"position"]) {
            Vector2 pos;
            
            pos.x = [dict[@"position"][@"x"] floatValue] * platformScale;
            pos.y = [dict[@"position"][@"y"] floatValue] * platformScale;
            
            elem->position = vector2Builder.add(pos);
        } else {
            elem->position = defaultPositionIndex_;
        }
        
        if (dict[@"scale"]) {
            Vector2 scale;
            
            scale.x = [dict[@"scale"][@"x"] floatValue];
            scale.y = [dict[@"scale"][@"y"] floatValue];
            
            elem->scale = vector2Builder.add(scale);
        } else {
            elem->scale = defaultScaleIndex_;
        }
        
        if (dict[@"hidden"]) {
            elem->hidden = [dict[@"hidden"] boolValue];
        } else {
            elem->hidden = false;
        }
        elem->rotation = [dict[@"rotation"] floatValue];
        elem->flags = [dict[@"flags"] unsignedIntValue];
    }
    
    strings_ = stringBuilder.createFixedArray();
    textures_ = texIntBuilder.createFixedArray();
    text_ = textIntBuilder.createFixedArray();
    animationSequences_ = animSeqIntBuilder.createFixedArray();
    placements_ = placementIntBuilder.createFixedArray();
    masks_ = maskIntBuilder.createFixedArray();
    textureMasks_ = textureMaskIntBuilder.createFixedArray();
    buttons_ = buttonIntBuilder.createFixedArray();
    buttonStates_ = buttonStateIntBuilder.createFixedArray();
    externalPackages_ = extPackageIntBuilder.createFixedArray();
    autoDisplayElements_ = autoDisplayElemIntBuilder.createFixedArray();
    
    rects_ = rectBuilder.createFixedArray();
    colorTransforms_ = colorXformBuilder.createFixedArray();
    colorMatrices_ = colorMtxBuilder.createFixedArray();
    vector2s_ = vector2Builder.createFixedArray();
    bounds_ = boundsRefIntBuilder.createFixedArray();
    keyframes_ = keyframeIntBuilder.createFixedArray();
    channels_ = channelRefIntBuilder.createFixedArray();

    // TODO: This will be done later perhaps?
    loadTextures([this, callback]() {
        loadFonts([this, callback]() {
            if (callback) {
                callback(this);
            }
        });
    });
}

void BGE::ScenePackage::loadTextures(std::function<void()> callback) {
    if (textureQueue_.size() > 0) {
        textureCount_->store(0);
        
        for (auto &tex : textureQueue_) {
            // Do we have subtextures affiliated with this?
            auto it = subTextures_.find(tex.first);
            
            if (it != subTextures_.end()) {
                Game::getInstance()->getTextureService()->createTextureAtlasFromFile(getHandle(), tex.first, tex.second, it->second, [this, callback](TextureAtlas *atlas, std::shared_ptr<Error> error) -> void {
                    int val = textureCount_->fetch_add(1) + 1;
                    
                    NSLog(@"Loaded atlas %s (%d)", atlas->getName().c_str(), (int)val);
                    if (val == textureQueue_.size()) {
                        if (callback) {
                            callback();
                        }
                    }
                });
            } else {
                Game::getInstance()->getTextureService()->createTextureFromFile(getHandle(), tex.first, tex.second, [this, callback](Texture *texture, std::shared_ptr<Error> error) -> void {
                    int val = textureCount_->fetch_add(1) + 1;
                    
                    NSLog(@"Loaded %s (%d)", texture->getName().c_str(), (int)val);
                    if (val == textureQueue_.size()) {
                        if (callback) {
                            callback();
                        }
                    }
                });
            }
        }
    } else if (callback) {
        callback();
    }
}

void BGE::ScenePackage::loadFonts(std::function<void()> callback) {
    if (fontQueue_.size() > 0) {
        fontCount_->store(0);
        
        for (auto &font : fontQueue_) {
            Game::getInstance()->getFontService()->createFont(font.first, font.second, handle_, [this, callback](FontHandle font, std::shared_ptr<Error> error) -> void {
                int val = fontCount_->fetch_add(1) + 1;
                
                if (val == fontQueue_.size()) {
                    if (callback) {
                        callback();
                    }
                }
            });
        }
    } else if (callback) {
        callback();
    }
}

BGE::ButtonReference *BGE::ScenePackage::getButtonReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = buttonNames_.baseAddress();
    
    for (auto i=0;i<buttonNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return buttonRefs_.addressOf(*buttonIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::MaskReference *BGE::ScenePackage::getMaskReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = maskNames_.baseAddress();
    
    for (auto i=0;i<maskNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return maskRefs_.addressOf(*maskIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::PlacementReference *BGE::ScenePackage::getPlacementReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = placementNames_.baseAddress();
    
    for (auto i=0;i<placementNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return placementRefs_.addressOf(*placementIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::TextureReference *BGE::ScenePackage::getTextureReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = textureNames_.baseAddress();
    
    for (auto i=0;i<textureNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return textureRefs_.addressOf(*textureIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::TextReference *BGE::ScenePackage::getTextReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = textNames_.baseAddress();
    
    for (auto i=0;i<textNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return textRefs_.addressOf(*textIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::AnimationSequenceReference *BGE::ScenePackage::getAnimationSequenceReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = animSeqNames_.baseAddress();
    
    for (auto i=0;i<animSeqNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return animSeqRefs_.addressOf(*animSeqIndices_.addressOf(i));
        }
    }
    
    
    return nullptr;
}

BGE::ExternalPackageReference *BGE::ScenePackage::getExternalReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = externalPackageNames_.baseAddress();
    
    for (auto i=0;i<externalPackageNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return externalPackageRefs_.addressOf(*externalPackageIndices_.addressOf(i));
        }
    }
    
    return nullptr;
}

BGE::GfxReferenceType BGE::ScenePackage::getReferenceType(std::string name) {
    auto it = referenceTypes_.find(name);
    
    if (it != referenceTypes_.end()) {
        return it->second;
    } else {
        return GfxReferenceTypeUnknown;
    }
}

BGE::GfxReferenceType BGE::ScenePackage::referenceTypeForString(std::string type) {
    GfxReferenceType value;
    
    if (type == "button") {
        value = GfxReferenceTypeButton;
    } else if (type == "externalReference") {
        value = GfxReferenceTypeExternalReference;
    } else if (type == "mask") {
        value = GfxReferenceTypeMask;
    } else if (type == "placement") {
        value = GfxReferenceTypePlacement;
    } else if (type == "sprite") {
        value = GfxReferenceTypeSprite;
    } else if (type == "symbol") {
        value = GfxReferenceTypeAnimationSequence;
    } else if (type == "symbolFrame") {
        value = GfxReferenceTypeKeyframe;
    } else if (type == "text") {
        value = GfxReferenceTypeText;
    } else if (type == "textureMask") {
        value = GfxReferenceTypeTextureMask;
    } else {
        value = GfxReferenceTypeUnknown;
    }

    return value;
}
