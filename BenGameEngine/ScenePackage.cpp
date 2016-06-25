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

BGE::ScenePackage::ScenePackage(uint64_t sceneId) : Object(sceneId), frameRate_(0), width_(0), height_(0), fontsLoaded_(false), texturesLoaded_(false), hasExternal_(false), defaultPositionIndex_(NullPtrIndex), defaultScaleIndex_(NullPtrIndex), fontCount_(nullptr) {
    ArrayBuilder<int, int> b;
    b.add(10);
    StringArrayBuilder c;
    c.add("Hello");
    
    position_ = Vector2{0, 0};
    textureCount_ = std::make_shared<std::atomic_int>(0);
    fontCount_ = std::make_shared<std::atomic_int>(0);
}

BGE::ScenePackage::~ScenePackage() {
    reset();
}

void BGE::ScenePackage::reset() {
    textures_.clear();
    text_.clear();
    animationSequences_.clear();
}

void BGE::ScenePackage::link() {
    // Link Texture
    std::shared_ptr<TextureService> textureService = BGE::Game::getInstance()->getTextureService();
    textureNames_ = FixedArray<const char*>(textures_.size());
    textureIndices_ = FixedArray<int32_t>(textures_.size());
    textureRefs_ = FixedArray<TextureReference>(textures_.size());
    
    for (auto i=0;i<textures_.size();i++) {
        TextureReferenceIntermediate *texRefInt = textures_.addressOf(i);
        const char**name = textureNames_.addressOf(i);
        int32_t *index = textureIndices_.addressOf(i);
        TextureReference *texRef = textureRefs_.addressOf(i);
        
        // Fix names
        const char* resolvedName = texRefInt->name + strings_.addressOf(0);
        *name = resolvedName;
        *index = i;
        texRef->name = resolvedName;
        texRef->texture = textureService->textureWithName(resolvedName);
    }

    // Link TextReference
    std::shared_ptr<FontService> fontService = BGE::Game::getInstance()->getFontService();
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
        
        textRef->font = fontService->getFont(fontName, textRefInt->size);
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
        animChannelRef->animation.frame = animChannelIntRef->frame;
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
    }
}

void BGE::ScenePackage::load(NSDictionary *jsonDict, std::function<void(ScenePackage *)> callback) {
    reset();
    float platformScale = 2.0;
    NSArray *textures = jsonDict[@"textures"];
    StringArrayBuilder stringBuilder ;
    ArrayBuilder<TextureReferenceIntermediate, TextureReferenceIntermediate> texIntBuilder;
    ArrayBuilder<TextReferenceIntermediate, TextReferenceIntermediate> textIntBuilder;
    ArrayBuilder<AnimationSequenceReferenceIntermediate, AnimationSequenceReferenceIntermediate> animSeqIntBuilder;
    
    UniqueArrayBuilder<Rect, Rect> rectBuilder;
    UniqueArrayBuilder<ColorTransform, ColorTransform> colorXformBuilder;
    UniqueArrayBuilder<ColorMatrix, ColorMatrix> colorMtxBuilder;
    UniqueArrayBuilder<Vector2, Vector2> vector2Builder;

    ArrayBuilder<BoundsReferenceIntermediate, BoundsReferenceIntermediate> boundsRefIntBuilder;
    ArrayBuilder<AnimationKeyframeReferenceIntermediate, AnimationKeyframeReferenceIntermediate> keyframeIntBuilder;
    ArrayBuilder<AnimationChannelReferenceIntermediate, AnimationChannelReferenceIntermediate> channelRefIntBuilder;
    
    setName([jsonDict[@"name"] UTF8String]);
    width_ = [jsonDict[@"width"] floatValue] * platformScale;
    height_ = [jsonDict[@"height"] floatValue] * platformScale;
    source_ = [jsonDict[@"source"] UTF8String];
    frameRate_ = [jsonDict[@"frameRate"] floatValue];
    position_.x = [jsonDict[@"position"][@"x"] floatValue] * platformScale;
    position_.y = [jsonDict[@"position"][@"y"] floatValue] * platformScale;
    
    defaultPositionIndex_ = vector2Builder.add(Vector2{0, 0});
    defaultScaleIndex_ = vector2Builder.add(Vector2{1, 1});
    
    texIntBuilder.resize(textures.count);
    
    for (NSInteger index=0;index<textures.count;index++) {
        NSDictionary *texDict = textures[index];
        TextureReferenceIntermediate *texRef = texIntBuilder.addressOf(index);
        
        // Add name
        const char* name = [texDict[@"name"] UTF8String];
        
        if (name) {
            texRef->name = stringBuilder.add(name);
            
            texRef->width = [texDict[@"width"] floatValue] * platformScale;
            texRef->height = [texDict[@"height"] floatValue] * platformScale;
            
            // Now get the file information
            NSString* filename = [[NSBundle mainBundle] pathForResource:texDict[@"filename"] ofType:nil];
            
            if (filename) {
                textureQueue_.push_back(std::make_pair<std::string, std::string>(name, [filename UTF8String]));
                Game::getInstance()->getTextureService()->namedTextureFromFile(name, [filename UTF8String], [texRef, name](std::shared_ptr<TextureBase> texture, std::shared_ptr<Error> error) -> void {
                    NSLog(@"%s: texture %x", name, texture.get());
                    
                    //texRef->texture = texture.get();
                });
            }
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    NSArray *text = jsonDict[@"text"];
    
    textIntBuilder.resize(text.count);
    
    for (NSInteger index=0;index<text.count;index++) {
        NSDictionary *textDict = text[index];
        TextReferenceIntermediate *textRef = textIntBuilder.addressOf(index);
        
        const char *name = [textDict[@"name"] UTF8String];
        const char *text = [textDict[@"text"] UTF8String];
        
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
            
#if 0
            BGE::Game::getInstance()->getFontService()->loadFont(fontName, size, [fontName, name, size, textRef](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
                NSLog(@"%s: font (%s:%d) %x", name, fontName, size, font.get());
                //textRef->font = font.get();
            });
#endif
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    NSArray *symbols = jsonDict[@"symbols"];
    
    animSeqIntBuilder.resize(symbols.count);
    
    for (NSInteger index=0;index<symbols.count;index++) {
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
                    
                    if (childName) {
                        auto channelIndexIt = channelIndex.find(childName);
                        AnimationChannelReferenceIntermediate *channel;
                        if (channelIndexIt != channelIndex.end()) {
                            channel = channelRefIntBuilder.addressOf(channelIndexIt->second);
                        } else {
                            AnimationChannelReferenceIntermediate newChannel;
                            const char* reference = [childDict[@"reference"] UTF8String];
                            
                            newChannel.name = stringBuilder.add(childName);
                            newChannel.reference = stringBuilder.add(reference);
                            
                            NSString *type = childDict[@"referenceType"];
                            GfxReferenceType value;
                            
                            newChannel.frame = 0;
                            
                            if ([type isEqualToString:@"button"]) {
                                value = GfxReferenceTypeButton;
                            } else if ([type isEqualToString:@"external"]) {
                                value = GfxReferenceTypeExternalReference;
                            } else if ([type isEqualToString:@"mask"]) {
                                value = GfxReferenceTypeMask;
                            } else if ([type isEqualToString:@"placement"]) {
                                value = GfxReferenceTypePlacement;
                            } else if ([type isEqualToString:@"sprite"]) {
                                value = GfxReferenceTypeSprite;
                            } else if ([type isEqualToString:@"symbol"]) {
                                value = GfxReferenceTypeAnimationSequence;
                            } else if ([type isEqualToString:@"symbolFrame"]) {
                                value = GfxReferenceTypeKeyframe;
                                
                                newChannel.frame = [childDict[@"frame"] unsignedIntValue];
                            } else if ([type isEqualToString:@"text"]) {
                                value = GfxReferenceTypeText;
                            } else if ([type isEqualToString:@"textureMask"]) {
                                value = GfxReferenceTypeTextureMask;
                            } else {
                                value = GfxReferenceTypeUnknown;
                            }
                            
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
                                    for (NSInteger cmi=0;cmi<rawMatrix.count;cmi++) {
                                        colorMatrixBacking.c[cmi] = [rawMatrix[cmi] floatValue];
                                    }
                                    
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
                    } else {
                        // TODO: How to handle non-named items
                        assert(name);
                    }
                }
            }
           
            boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currRect)});
            
            animSeq->numBounds = (uint32_t) boundsRefIntBuilder.size() - (uint32_t) animSeq->bounds;
            animSeq->numChannels = (uint32_t) channelRefIntBuilder.size() - animSeq->channels;
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    strings_ = stringBuilder.createFixedArray();
    textures_ = texIntBuilder.createFixedArray();
    text_ = textIntBuilder.createFixedArray();
    animationSequences_ = animSeqIntBuilder.createFixedArray();
    
    
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
    textureCount_->store(0);
    
    for (auto &tex : textureQueue_) {
        Game::getInstance()->getTextureService()->namedTextureFromFile(tex.first, tex.second, [this, callback](std::shared_ptr<TextureBase> texture, std::shared_ptr<Error> error) -> void {
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

void BGE::ScenePackage::loadFonts(std::function<void()> callback) {
    fontCount_->store(0);
    
    for (auto &font : fontQueue_) {
        BGE::Game::getInstance()->getFontService()->loadFont(font.first, font.second, [this, callback](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
            int val = fontCount_->fetch_add(1) + 1;
            
            if (val == fontQueue_.size()) {
                if (callback) {
                    callback();
                }
            }
        });
    }
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

