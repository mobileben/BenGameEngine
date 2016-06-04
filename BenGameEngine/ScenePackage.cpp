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
        if (tex.texture) {
            tex.texture = nullptr;
        }
    }
    
    for (auto &text : text_) {
        if (text.font) {
            text.font = nullptr;
        }
    }
    
    textures_.clear();
    text_.clear();
    animationSequences_.clear();
}

void BGE::ScenePackage::load(NSDictionary *jsonDict) {
    reset();
    
    NSArray *textures = jsonDict[@"textures"];
    StringArrayBuilder strAB;
    ArrayBuilder<TextureReference, TextureReference> texAB;
    ArrayBuilder<TextReference, TextReference> textAB;
    ArrayBuilder<AnimationSequenceReferenceBuilder, AnimationSequenceReferenceBuilder> animSeqArrBuilder;
    ArrayBuilder<AnimationKeyFrameTemp, AnimationKeyFrameTemp> keyFrameTempAB;
    ArrayBuilder<AnimationKeyFrameReference, AnimationKeyFrameReference> keyFrameAB;
    ArrayBuilder<AnimationChannelReference, AnimationChannelReference> animChannelRefAB;
    ArrayBuilder<Rect, Rect> rectChannelRefArrBuilder;
    ArrayBuilder<BoundsReferenceBuilder, BoundsReferenceBuilder> boundsRefArrBuilder;
    
    NSLog(@"Textures are %@", textures);
    NSLog(@"IS POD %d", std::is_pod<TextureReference>::value);
    NSLog(@"IS POD %d", std::is_pod<TextReference>::value);
    
    setName([jsonDict[@"name"] UTF8String]);
    width_ = [jsonDict[@"width"] floatValue];
    height_ = [jsonDict[@"height"] floatValue];
    source_ = [jsonDict[@"source"] UTF8String];
    frameRate_ = [jsonDict[@"frameRate"] floatValue];
    position_.x = [jsonDict[@"position"][@"x"] floatValue];
    position_.y = [jsonDict[@"position"][@"y"] floatValue];
    
    texAB.resize(textures.count);
    
    for (NSInteger index=0;index<textures.count;index++) {
        NSDictionary *texDict = textures[index];
        TextureReference *texRef = texAB.addressOf(index);
        
        // Add name
        const char* name = [texDict[@"name"] UTF8String];
        
        if (name) {
            size_t start;
            
            start = strAB.add(name);

            texRef->name = (const char *)start;
            
            texRef->width = [texDict[@"width"] floatValue];
            texRef->height = [texDict[@"height"] floatValue];
            
            // Now get the file information
            NSString* filename = [[NSBundle mainBundle] pathForResource:texDict[@"filename"] ofType:nil];
            
            if (filename) {
                Game::getInstance()->getTextureService()->namedTextureFromFile(name, [filename UTF8String], [texRef, name](std::shared_ptr<TextureBase> texture, std::shared_ptr<Error> error) -> void {
                    NSLog(@"%s: texture %x", name, texture.get());
                    
                    texRef->texture = texture.get();
                });
            }
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    NSArray *text = jsonDict[@"text"];
    
    textAB.resize(text.count);
    
    for (NSInteger index=0;index<text.count;index++) {
        NSDictionary *textDict = text[index];
        TextReference *textRef = textAB.addressOf(index);
        
        const char *name = [textDict[@"name"] UTF8String];
        
        if (name) {
            size_t start;
            NSDictionary *formatDict = textDict[@"format"];
            
            start = strAB.add(name);
            
            textRef->name = (const char *)start;
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
            std::string fontName = [formatDict[@"fontName"] UTF8String];
            BGE::Game::getInstance()->getFontService()->loadFont(fontName, size, [fontName, name, size, textRef](std::shared_ptr<Font> font, std::shared_ptr<Error> error) -> void {
                NSLog(@"%s: font (%s:%d) %x", name, fontName.c_str(), size, font.get());
                textRef->font = font.get();
            });
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    NSArray *symbols = jsonDict[@"symbols"];
    
    animSeqArrBuilder.resize(symbols.count);
    
    for (NSInteger index=0;index<symbols.count;index++) {
        NSDictionary *symbolDict = symbols[index];
        AnimationSequenceReferenceBuilder *animSeq = animSeqArrBuilder.addressOf(index);
        
        const char *name = [symbolDict[@"name"] UTF8String];
        
        if (name) {
            size_t start;
            
            start = strAB.add(name);

            animSeq->name = (const char *)start;
            animSeq->frameRate = frameRate_;
            
            NSArray *frames = symbolDict[@"frames"];
            std::vector<AnimationChannelReferenceBuilder> channels;
            std::vector<AnimationKeyFrameReference> keyFrames;
            std::vector<BoundsReference> bounds;
            std::map<std::string, unsigned long> channelIndex;
            
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
                        // This is truly new
                        bounds.push_back(currBounds);
                        currBounds = newBounds;
                    } else {
                        currBounds.totalFrames++;
                    }
                }
                
                for (NSInteger ci=0;ci<children.count;ci++) {
                    NSDictionary *childDict = children[ci];
                    const char *childName = [childDict[@"name"] UTF8String];
                    
                    if (childName) {
                        auto channelIndexIt = channelIndex.find(childName);
                        AnimationChannelReferenceBuilder *channel;
                        if (channelIndexIt != channelIndex.end()) {
                            channel = &channels[channelIndexIt->second];
                        } else {
                            size_t start;
                            AnimationChannelReferenceBuilder newChannel;
                            
                            start = strAB.add(childName);
                            newChannel.name = (const char *)start;
                            
                            const char* reference = [childDict[@"reference"] UTF8String];
                            
                            start = strAB.add(reference);
                            
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
                            
                            unsigned long idx = channels.size();
                            channelIndex[childName] = idx;
                            channels.push_back(newChannel);
                            channel = &channels[idx];
                        }
                        
                        // Handle keyframe
                        if (channel->keyFrames.size() == 0) {
                            AnimationKeyFrameReference keyFrame;
                            keyFrame.startFrame = fi;
                            keyFrame.totalFrames = 1;
                            keyFrame.order = ci;
                            keyFrame.flags = [childDict[@"flags"] unsignedIntValue];
                            
                            // Bounds
                            keyFrame.bounds.x = [childDict[@"bounds"][@"x"] floatValue];
                            keyFrame.bounds.y = [childDict[@"bounds"][@"y"] floatValue];
                            keyFrame.bounds.w = [childDict[@"bounds"][@"width"] floatValue];
                            keyFrame.bounds.h = [childDict[@"bounds"][@"height"] floatValue];

                            // Color
                            if (childDict[@"color"]) {
                                keyFrame.colorTransform.multiplier.r = [childDict[@"color"][@"redMultiplier"] floatValue];
                                keyFrame.colorTransform.multiplier.g = [childDict[@"color"][@"greenMultiplier"] floatValue];
                                keyFrame.colorTransform.multiplier.b = [childDict[@"color"][@"blueMultiplier"] floatValue];
                                keyFrame.colorTransform.multiplier.a = [childDict[@"color"][@"alphaMultiplier"] floatValue];
                                keyFrame.colorTransform.offset.r = [childDict[@"color"][@"redOffset"] floatValue];
                                keyFrame.colorTransform.offset.g = [childDict[@"color"][@"greenOffset"] floatValue];
                                keyFrame.colorTransform.offset.b = [childDict[@"color"][@"blueOffset"] floatValue];
                                keyFrame.colorTransform.offset.a = [childDict[@"color"][@"alphaOffset"] floatValue];
                                keyFrame.colorTransform.color = [childDict[@"color"][@"color"] floatValue];
                            } else {
                                keyFrame.colorTransform.multiplier.r = 0;
                                keyFrame.colorTransform.multiplier.g = 0;
                                keyFrame.colorTransform.multiplier.b = 0;
                                keyFrame.colorTransform.multiplier.a = 0;
                                keyFrame.colorTransform.offset.r = 0;
                                keyFrame.colorTransform.offset.g = 0;
                                keyFrame.colorTransform.offset.b = 0;
                                keyFrame.colorTransform.offset.a = 0;
                                keyFrame.colorTransform.color = 0;
                            }
                            // Filters
                            Matrix4MakeIdentify(keyFrame.colorMatrix.matrix);
                            keyFrame.colorMatrix.offset = { 0, 0, 0, 0 };

                            if (childDict[@"filters"]) {
                                NSArray *filters = childDict[@"filters"];
                                
                                for (NSDictionary *filter in filters) {
                                    NSArray *colorMatrix = filter[@"ColorMatrixFilter"][@"matrix"];
                                    
                                    if (colorMatrix) {
                                        for (NSInteger cmi=0;cmi<colorMatrix.count;cmi++) {
                                            keyFrame.colorMatrix.c[cmi] = [colorMatrix[cmi] floatValue];
                                        }
                                    }
                                }
                            }
                            
                            // Position
                            if (childDict[@"position"]) {
                                keyFrame.position.x = [childDict[@"position"][@"x"] floatValue];
                                keyFrame.position.y = [childDict[@"position"][@"y"] floatValue];
                            } else {
                                keyFrame.position.x = 0;
                                keyFrame.position.y = 0;
                            }
                            
                            // Scale
                            if (childDict[@"scale"]) {
                                keyFrame.scale.x = [childDict[@"scale"][@"x"] floatValue];
                                keyFrame.scale.y = [childDict[@"scale"][@"y"] floatValue];
                            } else {
                                keyFrame.scale.x = 1;
                                keyFrame.scale.y = 1;
                            }
                            
                            // Rotation
                            if (childDict[@"rotation"]) {
                                keyFrame.rotation = [childDict[@"rotation"] floatValue];
                            } else {
                                keyFrame.rotation = 0;
                            }
                            
                            // Now build matrix
                            
                            channel->keyFrames.push_back(keyFrame);
                        } else {
                            
                        }
                        
                        NSLog(@"%p", channel);
                    } else {
                        // TODO: How to handle non-named items
                        assert(name);
                    }

                }
            }
            
            bounds.push_back(currBounds);
        } else {
            // TODO: How to handle non-named items
            assert(name);
        }
    }
    
    strings_ = strAB.createFixedArray();
    textures_ = texAB.createFixedArray();
    text_ = textAB.createFixedArray();
    
    ArrayBuilder<AnimationSequenceReference, AnimationSequenceReference> animSeqRefBuilder;
    
    animSeqRefBuilder.resize(animSeqArrBuilder.size());
    
    auto index = 0;
    
    for (auto& animSeq : animSeqArrBuilder) {
        AnimationSequenceReference *animSeqRef = animSeqRefBuilder.addressOf(index);
        index++;
    }
    animationSequences_ = animSeqRefBuilder.createFixedArray();
    
    NSLog(@"Size is %ld", strAB.size());
    
    // Fix all string pointers
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
    
    
    NSLog(@"DONE HERE BITCH");
}