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

BGE::ScenePackage::ScenePackage(uint64_t sceneId) : Object(sceneId) {
    ArrayBuilder<int, int> b;
    b.add(10);
    StringArrayBuilder c;
    c.add("Hello");
}

BGE::ScenePackage::~ScenePackage() {
    reset();
}

void BGE::ScenePackage::reset() {
    for (auto &tex : textures_) {
    }
    
    for (auto &text : text_) {
    }
    
    textures_.clear();
    text_.clear();
    animationSequences_.clear();
}

void BGE::ScenePackage::prelink() {
    // Fix all string pointers
#if 0
    for (auto& tex : textures_) {
        long offset = (long) tex.name;
        //        tex.name = strs_ + offset;
        tex.name = strings_.addressOf(offset);
        NSLog(@"Tex is %s", tex.name);
    }
    
    for (auto& text : text_) {
        long offset = (long) text.name;
        text.name = strings_.addressOf(offset);
        NSLog(@"Text is %s", text.name);
    }
    
    for (auto& animSeq : animationSequences_) {
        long offset = (long) animSeq.name;
        animSeq.name = strings_.addressOf(offset);
        NSLog(@"Anim Seq is %s", animSeq.name);
    }
#endif
}

void BGE::ScenePackage::link() {
}

void BGE::ScenePackage::load(NSDictionary *jsonDict) {
    reset();
    
    NSArray *textures = jsonDict[@"textures"];
    StringArrayBuilder stringBuilder;
    ArrayBuilder<TextureReferenceIntermediate, TextureReferenceIntermediate> texIntBuilder;
    ArrayBuilder<TextReferenceIntermediate, TextReferenceIntermediate> textIntBuilder;
    ArrayBuilder<AnimationSequenceReferenceIntermediate, AnimationSequenceReferenceIntermediate> animSeqIntBuilder;
    
    UniqueArrayBuilder<Rect, Rect> rectBuilder;
    UniqueArrayBuilder<ColorTransform, ColorTransform> colorXformBuilder;
    UniqueArrayBuilder<ColorMatrix, ColorMatrix> colorMtxBuilder;
    UniqueArrayBuilder<Vector2, Vector2> vector2Builder;

    ArrayBuilder<BoundsReferenceIntermediate, BoundsReferenceIntermediate> boundsRefIntBuilder;
    ArrayBuilder<AnimationKeyFrameReferenceIntermediate, AnimationKeyFrameReferenceIntermediate> keyframeIntBuilder;
    ArrayBuilder<AnimationChannelReferenceIntermediate, AnimationChannelReferenceIntermediate> channelRefIntBuilder;
    
    setName([jsonDict[@"name"] UTF8String]);
    width_ = [jsonDict[@"width"] floatValue];
    height_ = [jsonDict[@"height"] floatValue];
    source_ = [jsonDict[@"source"] UTF8String];
    frameRate_ = [jsonDict[@"frameRate"] floatValue];
    position_.x = [jsonDict[@"position"][@"x"] floatValue];
    position_.y = [jsonDict[@"position"][@"y"] floatValue];
    
    texIntBuilder.resize(textures.count);
    
    for (NSInteger index=0;index<textures.count;index++) {
        NSDictionary *texDict = textures[index];
        TextureReferenceIntermediate *texRef = texIntBuilder.addressOf(index);
        
        // Add name
        const char* name = [texDict[@"name"] UTF8String];
        
        if (name) {
            texRef->name = stringBuilder.add(name);
            
            texRef->width = [texDict[@"width"] floatValue];
            texRef->height = [texDict[@"height"] floatValue];
            
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
        
        if (name) {
            NSDictionary *formatDict = textDict[@"format"];
            
            textRef->name = stringBuilder.add(name);
            textRef->width = [textDict[@"width"] floatValue];
            textRef->height = [textDict[@"height"] floatValue];
            textRef->leading = [formatDict[@"leading"] floatValue];
            
            std::string alignment = [formatDict[@"alignment"] UTF8String];
            
            if (alignment == "left"){
                textRef->alignment = FontHorizontalAlignment::Left;
            } else if (alignment == "right"){
                textRef->alignment = FontHorizontalAlignment::Right;
            } else {
                textRef->alignment = FontHorizontalAlignment::Center;
            }
            
            uint32_t size = [formatDict[@"fontSize"] unsignedIntValue];
            const char* fontName = [formatDict[@"fontName"] UTF8String];
            
            fontQueue_.push_back(std::make_pair<std::string, int32_t>(fontName, size));
                           
            BGE::Game::getInstance()->getFontService()->loadFont(fontName, size, [fontName, name, size, textRef](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
                NSLog(@"%s: font (%s:%d) %x", name, fontName, size, font.get());
                //textRef->font = font.get();
            });
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
            size_t start;
            
            start = stringBuilder.add(name);

            animSeq->name = (const char *)start;
            animSeq->frameRate = frameRate_;
            animSeq->bounds = (uint32_t) boundsRefIntBuilder.size();
            animSeq->channels = (uint32_t) channelRefIntBuilder.size();
            
            NSArray *frames = symbolDict[@"frames"];
            std::map<std::string, int32_t> channelIndex;
            
            BoundsReference currBounds;
            BoundsReference newBounds;
            
            currBounds.startFrame = 0;
            currBounds.totalFrames = 1;
        
            for (NSInteger fi=0;fi<frames.count;fi++) {
                NSDictionary *frameDict = frames[fi];
                NSArray *children = frameDict[@"children"];
                
                if (fi == 0) {
                    currBounds.bounds.x = [frameDict[@"bounds"][@"x"] floatValue];
                    currBounds.bounds.y = [frameDict[@"bounds"][@"y"] floatValue];
                    currBounds.bounds.w = [frameDict[@"bounds"][@"width"] floatValue];
                    currBounds.bounds.h = [frameDict[@"bounds"][@"height"] floatValue];
                } else {
                    newBounds.startFrame = 0;
                    newBounds.totalFrames = 1;
                    newBounds.bounds.x = [frameDict[@"bounds"][@"x"] floatValue];
                    newBounds.bounds.y = [frameDict[@"bounds"][@"y"] floatValue];
                    newBounds.bounds.w = [frameDict[@"bounds"][@"width"] floatValue];
                    newBounds.bounds.h = [frameDict[@"bounds"][@"height"] floatValue];
                    
                    
                    if (newBounds.bounds.x != currBounds.bounds.x || newBounds.bounds.y != currBounds.bounds.y || newBounds.bounds.w != currBounds.bounds.w || newBounds.bounds.h != currBounds.bounds.h) {
                        currBounds = newBounds;
                        boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currBounds.bounds)});
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
                            size_t start;
                            AnimationChannelReferenceIntermediate newChannel;
                            
                            start = stringBuilder.add(childName);
                            newChannel.name = (const char *)start;
                            
                            const char* reference = [childDict[@"reference"] UTF8String];
                            
                            start = stringBuilder.add(reference);
                            
                            newChannel.reference = (const char *)start;
                            
                            NSString *type = childDict[@"referenceType"];
                            GfxReferenceType value;
                            
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
                                value = GfxReferenceTypeKeyFrame;
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
                        
                        AnimationKeyFrameReferenceIntermediate keyFrame;
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

                        keyFrame.startFrame = (uint32_t)fi;
                        keyFrame.totalFrames = 1;
                        keyFrame.order = (uint32_t)ci;
                        keyFrame.flags = [childDict[@"flags"] unsignedIntValue];
                        
                        // Bounds
                        
                        boundsBacking.x = [childDict[@"bounds"][@"x"] floatValue];
                        boundsBacking.y = [childDict[@"bounds"][@"y"] floatValue];
                        boundsBacking.w = [childDict[@"bounds"][@"width"] floatValue];
                        boundsBacking.h = [childDict[@"bounds"][@"height"] floatValue];
                        
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
                            
                            positionBacking.x = [childDict[@"position"][@"x"] floatValue];
                            positionBacking.y = [childDict[@"position"][@"y"] floatValue];
                            
                            keyFrame.position = (int32_t) vector2Builder.add(positionBacking);
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
                            keyFrame.rotation = [childDict[@"rotation"] floatValue];
                        } else {
                            keyFrame.rotation = 0;
                        }
                        
                        // TODO: Now build matrix

                        // Now determine if this keyframe is a new keyframe
                        bool isNew = false;
                        AnimationKeyFrameReferenceIntermediate *currKeyFrame = nullptr;
                        int32_t keyFrameIndex;

                        if (channel->keyFrames.size() == 0) {
                            isNew = true;
                        } else {
                            keyFrameIndex = channel->keyFrames[channel->keyFrames.size() - 1];
                            
                            AnimationKeyFrameReferenceIntermediate *currKeyFrame = &keyframeIntBuilder[keyFrameIndex];
                            
                            // Now we need to determine if this is a new keyframe or existing
                            if (currKeyFrame->order != keyFrame.order) {
                                isNew = true;
                            } else if (currKeyFrame->flags != keyFrame.flags) {
                                isNew = true;
                            } else {
                                Rect *currBounds;
                                ColorTransform *currColorTransform;
                                ColorMatrix *currColorMatrix;
                                Vector2 *currPosition;
                                Vector2 *currScale;

                                currBounds = rectBuilder.safeAddressOf(currKeyFrame->bounds);
                                currColorTransform = colorXformBuilder.safeAddressOf(currKeyFrame->colorTransform);
                                currColorMatrix = colorMtxBuilder.safeAddressOf(currKeyFrame->colorMatrix);
                                currPosition = vector2Builder.safeAddressOf(currKeyFrame->position);
                                currScale = vector2Builder.safeAddressOf(currKeyFrame->scale);
                                
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
                            // This is a new keyframe, key values already set, but if needed, we need to assign the offsets
                            keyFrame.bounds = rectBuilder.add(boundsBacking);
                            
                            if (position) {
                                keyFrame.position = vector2Builder.add(*position);
                            } else {
                                keyFrame.position = NullPtrIndex;
                            }
                            
                            if (scale) {
                                keyFrame.scale = vector2Builder.add(*scale);
                            } else {
                                keyFrame.scale = NullPtrIndex;
                            }
                            
                            if (colorTransform) {
                                keyFrame.colorTransform = colorXformBuilder.add(*colorTransform);
                            } else {
                                keyFrame.colorTransform = NullPtrIndex;
                            }
                            
                            if (colorMatrix) {
                                keyFrame.colorMatrix = colorMtxBuilder.add(*colorMatrix);
                            } else {
                                keyFrame.colorMatrix = NullPtrIndex;
                            }
                            
                            keyFrameIndex = keyframeIntBuilder.add(keyFrame);

                            channel->keyFrames.push_back(keyFrameIndex);
                        } else if (currKeyFrame) {
                            currKeyFrame->totalFrames++;
                        } else {
                            assert(false);
                        }
                    } else {
                        // TODO: How to handle non-named items
                        assert(name);
                    }
                }
            }
           
            boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currBounds.bounds)});
            
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
    boundRefs_ = boundsRefIntBuilder.createFixedArray();
    keyframes_ = keyframeIntBuilder.createFixedArray();
    channels_ = channelRefIntBuilder.createFixedArray();

    // TODO: This will be done later perhaps?
    prelink();
    
    NSLog(@"Size is %ld", stringBuilder.size());
    
    
    
    NSLog(@"DONE HERE BITCH");
}