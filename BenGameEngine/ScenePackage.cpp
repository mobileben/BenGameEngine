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
#include "rapidjson/document.h"
#include <type_traits>
#include <cstdlib>
#include <future>

#import <Foundation/Foundation.h>

const char *BGE::kScenePackageKeyAlignment = "alignment";
const char *BGE::kScenePackageKeyAlphaMultiplier = "alphaMultiplier";
const char *BGE::kScenePackageKeyAlphaOffset = "alphaOffset";
const char *BGE::kScenePackageKeyAtlas = "atlas";
const char *BGE::kScenePackageKeyAutoDisplayList = "autoDisplayList";
const char *BGE::kScenePackageKeyBlueMultiplier = "blueMultiplier";
const char *BGE::kScenePackageKeyBlueOffset = "blueOffset";
const char *BGE::kScenePackageKeyBounds = "bounds";
const char *BGE::kScenePackageKeyButtons = "buttons";
const char *BGE::kScenePackageKeyCenter = "center";
const char *BGE::kScenePackageKeyChildren = "children";
const char *BGE::kScenePackageKeyCollRectScale = "collRectScale";
const char *BGE::kScenePackageKeyColor = "color";
const char *BGE::kScenePackageKeyColorMatrixFilter = "ColorMatrixFilter";
const char *BGE::kScenePackageKeyExternalPackage = "externalPackage";
const char *BGE::kScenePackageKeyExternalReferences = "externalReferences";
const char *BGE::kScenePackageKeyFilename = "filename";
const char *BGE::kScenePackageKeyFilters = "filters";
const char *BGE::kScenePackageKeyFlags = "flags";
const char *BGE::kScenePackageKeyFontColor = "fontColor";
const char *BGE::kScenePackageKeyFontName = "fontName";
const char *BGE::kScenePackageKeyFontSize = "fontSize";
const char *BGE::kScenePackageKeyFormat = "format";
const char *BGE::kScenePackageKeyFrame = "frame";
const char *BGE::kScenePackageKeyFrameRate = "frameRate";
const char *BGE::kScenePackageKeyFrames = "frames";
const char *BGE::kScenePackageKeyGreenMultiplier = "greenMultiplier";
const char *BGE::kScenePackageKeyGreenOffset = "greenOffset";
const char *BGE::kScenePackageKeyHeight = "height";
const char *BGE::kScenePackageKeyHidden = "hidden";
const char *BGE::kScenePackageKeyLeading = "leading";
const char *BGE::kScenePackageKeyLeft = "left";
const char *BGE::kScenePackageKeyMasks = "masks";
const char *BGE::kScenePackageKeyMatrix = "matrix";
const char *BGE::kScenePackageKeyMultiline = "multiline";
const char *BGE::kScenePackageKeyName = "name";
const char *BGE::kScenePackageKeyPixelsPerPoint = "pixelsPerPoint";
const char *BGE::kScenePackageKeyPlacements = "placements";
const char *BGE::kScenePackageKeyPosition = "position";
const char *BGE::kScenePackageKeyRedMultiplier = "redMultiplier";
const char *BGE::kScenePackageKeyRedOffset = "redOffset";
const char *BGE::kScenePackageKeyReference = "reference";
const char *BGE::kScenePackageKeyReferenceType = "referenceType";
const char *BGE::kScenePackageKeyRight = "right";
const char *BGE::kScenePackageKeyRotated = "rotated";
const char *BGE::kScenePackageKeyRotation = "rotation";
const char *BGE::kScenePackageKeyScale = "scale";
const char *BGE::kScenePackageKeySkew = "skew";
const char *BGE::kScenePackageKeySource = "source";
const char *BGE::kScenePackageKeyState = "state";
const char *BGE::kScenePackageKeyStates = "states";
const char *BGE::kScenePackageKeySubtextures = "subtextures";
const char *BGE::kScenePackageKeySymbols = "symbols";
const char *BGE::kScenePackageKeyText = "text";
const char *BGE::kScenePackageKeyTextureMasks = "textureMasks";
const char *BGE::kScenePackageKeyTextures = "textures";
const char *BGE::kScenePackageKeyWidth = "width";
const char *BGE::kScenePackageKeyX = "x";
const char *BGE::kScenePackageKeyY = "y";

const std::string BGE::ScenePackage::ErrorDomain = "ScenePackage";

const uint8_t BGE::kScenePackageFormatMagic[kScenePackageFormatHeaderMagicSize] = "BGESPKG";

////////////////////////////////////////////////////////////////////////////////
//
//  BGE::ScenePackageFormat
//
////////////////////////////////////////////////////////////////////////////////

BGE::ScenePackageFormat::ScenePackageFormat() : frameRate_(0), pixelsPerPoint_(0), width_(0), height_(0), position_{}, emptyStringIndex_(0), defaultPositionIndex_(0), defaultScaleIndex_(0), defaultSkewIndex_(0), defaultCollisionRectScaleIndex_(0), sourceIndex_(0), managed_(false), header_(nullptr), buffer_(nullptr), bufferSize_(0) {
}

BGE::ScenePackageFormat::ScenePackageFormat(const ScenePackage& package) : frameRate_(package.frameRate_), pixelsPerPoint_(package.pixelsPerPoint_), width_(package.width_), height_(package.height_), position_(package.position_), emptyStringIndex_(package.emptyStringIndex_), defaultPositionIndex_(package.defaultPositionIndex_), defaultScaleIndex_(package.defaultScaleIndex_), defaultSkewIndex_(package.defaultSkewIndex_), defaultCollisionRectScaleIndex_(package.defaultCollisionRectScaleIndex_), sourceIndex_(package.sourceIndex_), managed_(false), header_(nullptr), buffer_(nullptr), bufferSize_(0) {
    strings_ = package.strings_;
    atlasSubtextures_ = package.atlasSubtextures_;
    subTextureDefs_ = package.subTextureDefs_;
    textures_ = package.textures_;
    text_ = package.text_;
    animationSequences_ = package.animationSequences_;
    placements_ = package.placements_;
    masks_ = package.masks_;
    textureMasks_ = package.textureMasks_;
    buttons_ = package.buttons_;
    buttonStates_ = package.buttonStates_;
    externalPackages_ = package.externalPackages_;
    autoDisplayElements_ = package.autoDisplayElements_;

    rects_ = package.rects_;
    colorTransforms_ = package.colorTransforms_;
    colorMatrices_ = package.colorMatrices_;
    vector2s_ = package.vector2s_;
    keyframeIndices_ = package.keyframeIndices_;

    bounds_ = package.bounds_;
    keyframes_ = package.keyframes_;
    channels_ = package.channels_;
}

BGE::ScenePackageFormat::ScenePackageFormat(const uint64_t *buffer, size_t bufferSize, bool managed) : frameRate_(0), pixelsPerPoint_(0), width_(0), height_(0), position_{}, emptyStringIndex_(0), defaultPositionIndex_(0), defaultScaleIndex_(0), defaultSkewIndex_(0), defaultCollisionRectScaleIndex_(0), sourceIndex_(0), managed_(false), header_(nullptr), buffer_(nullptr), bufferSize_(0) {
    load(buffer, bufferSize, managed);
}

BGE::ScenePackageFormat::~ScenePackageFormat() {
    if (!managed_ && buffer_) {
        delete [] buffer_;
    }
}

bool BGE::ScenePackageFormat::isAligned8Memory(size_t size) const {
    return !(size&7);
}

size_t BGE::ScenePackageFormat::aligned8MemorySize(size_t size) const {
    size_t fixed = size;
    if (!isAligned8Memory(size)) {
        size_t tmp = size / 8;
        fixed = (tmp + 1) * 8;
    }
    return fixed;
}

void BGE::ScenePackageFormat::load(const uint64_t *buffer, size_t bufferSize, bool managed) {
    if (buffer) {
        buffer_ = const_cast<uint64_t *>(buffer);
        bufferSize_ = bufferSize;
        managed_ = managed;
        header_ = reinterpret_cast<ScenePackageFormatHeader *>(buffer_);

        // TODO: Test magic
        // TODO: Test endian
        // TODO: Test version

        frameRate_ = header_->frameRate;
        pixelsPerPoint_ = header_->pixelsPerPoint;
        width_ = header_->width;
        height_ = header_->height;
        position_ = header_->position;
        emptyStringIndex_ = header_->emptyStringIndex;
        defaultPositionIndex_ = header_->defaultPositionIndex;
        defaultScaleIndex_ = header_->defaultScaleIndex;
        defaultSkewIndex_ = header_->defaultSkewIndex;
        defaultCollisionRectScaleIndex_ = header_->defaultCollisionRectScaleIndex;
        sourceIndex_ = header_->sourceIndex;

        uint8_t *bytePtr = reinterpret_cast<uint8_t *>(buffer_);

        // Strings
        // offset of 0 means null
        if (header_->stringsOffset) {
            auto bufferPtr = reinterpret_cast<char *>(&bytePtr[header_->stringsOffset]);
            strings_ = FixedArray<char>(bufferPtr, header_->stringsSize, true);
        }

        // Atlas Subtextures
        // offset of 0 means null
        if (header_->atlasSubtexturesOffset) {
            auto bufferPtr = reinterpret_cast<TextureAtlasSubTextureDefIntermediate *>(&bytePtr[header_->atlasSubtexturesOffset]);
            atlasSubtextures_ = FixedArray<TextureAtlasSubTextureDefIntermediate>(bufferPtr, header_->atlasSubtexturesSize, true);
        }

        // Sub Texture Defs
        // offset of 0 means null
        if (header_->subTextureDefsOffset) {
            auto bufferPtr = reinterpret_cast<SubTextureDefIntermediate *>(&bytePtr[header_->subTextureDefsOffset]);
            subTextureDefs_ = FixedArray<SubTextureDefIntermediate>(bufferPtr, header_->subTextureDefsSize, true);
        }

        // Textures
        // offset of 0 means null
        if (header_->texturesOffset) {
            auto bufferPtr = reinterpret_cast<TextureReferenceIntermediate *>(&bytePtr[header_->texturesOffset]);
            textures_ = FixedArray<TextureReferenceIntermediate>(bufferPtr, header_->texturesSize, true);
        }

        // Text
        // offset of 0 means null
        if (header_->textOffset) {
            auto bufferPtr = reinterpret_cast<TextReferenceIntermediate *>(&bytePtr[header_->textOffset]);
            text_ = FixedArray<TextReferenceIntermediate>(bufferPtr, header_->textSize, true);
        }

        // Animation Sequences
        // offset of 0 means null
        if (header_->animationSequencesOffset) {
            auto bufferPtr = reinterpret_cast<AnimationSequenceReferenceIntermediate *>(&bytePtr[header_->animationSequencesOffset]);
            animationSequences_ = FixedArray<AnimationSequenceReferenceIntermediate>(bufferPtr, header_->animationSequencesSize, true);
        }

        // Placements
        // offset of 0 means null
        if (header_->placementsOffset) {
            auto bufferPtr = reinterpret_cast<PlacementIntermediate *>(&bytePtr[header_->placementsOffset]);
            placements_ = FixedArray<PlacementIntermediate>(bufferPtr, header_->placementsSize, true);
        }

        // Masks
        // offset of 0 means null
        if (header_->masksOffset) {
            auto bufferPtr = reinterpret_cast<MaskIntermediate *>(&bytePtr[header_->masksOffset]);
            masks_ = FixedArray<MaskIntermediate>(bufferPtr, header_->masksSize, true);
        }

        // Texture Masks
        // offset of 0 means null
        if (header_->textureMasksOffset) {
            auto bufferPtr = reinterpret_cast<TextureMaskIntermediate *>(&bytePtr[header_->textureMasksOffset]);
            textureMasks_ = FixedArray<TextureMaskIntermediate>(bufferPtr, header_->textureMasksSize, true);
        }

        // Buttons
        // offset of 0 means null
        if (header_->buttonsOffset) {
            auto bufferPtr = reinterpret_cast<ButtonIntermediate *>(&bytePtr[header_->buttonsOffset]);
            buttons_ = FixedArray<ButtonIntermediate>(bufferPtr, header_->buttonsSize, true);
        }

        // Button States
        // offset of 0 means null
        if (header_->buttonStatesOffset) {
            auto bufferPtr = reinterpret_cast<ButtonStateIntermediate *>(&bytePtr[header_->buttonStatesOffset]);
            buttonStates_ = FixedArray<ButtonStateIntermediate>(bufferPtr, header_->buttonStatesSize, true);
        }

        // External Packages
        // offset of 0 means null
        if (header_->externalPackagesOffset) {
            auto bufferPtr = reinterpret_cast<ExternalPackageIntermediate *>(&bytePtr[header_->externalPackagesOffset]);
            externalPackages_ = FixedArray<ExternalPackageIntermediate>(bufferPtr, header_->externalPackagesSize, true);
        }

        // Auto Display Elements
        // offset of 0 means null
        if (header_->autoDisplayElementsOffset) {
            auto bufferPtr = reinterpret_cast<AutoDisplayElementIntermediate *>(&bytePtr[header_->autoDisplayElementsOffset]);
            autoDisplayElements_ = FixedArray<AutoDisplayElementIntermediate>(bufferPtr, header_->autoDisplayElementsSize, true);
        }

        // Rects
        // offset of 0 means null
        if (header_->rectsOffset) {
            auto bufferPtr = reinterpret_cast<Rect *>(&bytePtr[header_->rectsOffset]);
            rects_ = FixedArray<Rect>(bufferPtr, header_->rectsSize, true);
        }

        // Color Tranforms
        // offset of 0 means null
        if (header_->colorTransformsOffset) {
            auto bufferPtr = reinterpret_cast<ColorTransform *>(&bytePtr[header_->colorTransformsOffset]);
            colorTransforms_ = FixedArray<ColorTransform>(bufferPtr, header_->colorTransformsSize, true);
        }

        // Color Matrices
        // offset of 0 means null
        if (header_->colorMatricesOffset) {
            auto bufferPtr = reinterpret_cast<ColorMatrix *>(&bytePtr[header_->colorMatricesOffset]);
            colorMatrices_ = FixedArray<ColorMatrix>(bufferPtr, header_->colorMatricesSize, true);
        }

        // Vector2s
        // offset of 0 means null
        if (header_->vector2sOffset) {
            auto bufferPtr = reinterpret_cast<Vector2 *>(&bytePtr[header_->vector2sOffset]);
            vector2s_ = FixedArray<Vector2>(bufferPtr, header_->vector2sSize, true);
        }

        // Key Frame Indices
        // offset of 0 means null
        if (header_->keyframeIndicesOffset) {
            auto bufferPtr = reinterpret_cast<int32_t *>(&bytePtr[header_->keyframeIndicesOffset]);
            keyframeIndices_ = FixedArray<int32_t>(bufferPtr, header_->keyframeIndicesSize, true);
        }

        // Bounds
        // offset of 0 means null
        if (header_->boundsOffset) {
            auto bufferPtr = reinterpret_cast<BoundsReferenceIntermediate *>(&bytePtr[header_->boundsOffset]);
            bounds_ = FixedArray<BoundsReferenceIntermediate>(bufferPtr, header_->boundsSize, true);
        }

        // Keyframes
        // offset of 0 means null
        if (header_->keyframesOffset) {
            auto bufferPtr = reinterpret_cast<AnimationKeyframeReferenceIntermediate *>(&bytePtr[header_->keyframesOffset]);
            keyframes_ = FixedArray<AnimationKeyframeReferenceIntermediate>(bufferPtr, header_->keyframesSize, true);
        }

        // Channels
        // offset of 0 means null
        if (header_->channelsOffset) {
            auto bufferPtr = reinterpret_cast<AnimationChannelReferenceIntermediate *>(&bytePtr[header_->channelsOffset]);
            channels_ = FixedArray<AnimationChannelReferenceIntermediate>(bufferPtr, header_->channelsSize, true);
        }
    }
}

void BGE::ScenePackageFormat::save(const std::string& filename) {
    if (!buffer_) {
        ScenePackageFormatHeader    header{};
        std::vector<size_t>         offsets;

        // Fill in parts of the header
        for (size_t i=0;i<sizeof(kScenePackageFormatMagic);++i) {
            header.magic[i] = kScenePackageFormatMagic[i];
        }

        header.endian = kScenePackageFormatEndian;
        header.version = kScenePackageFormatVersion;
        header.frameRate = frameRate_;
        header.pixelsPerPoint = pixelsPerPoint_;
        header.width = width_;
        header.height = height_;
        header.position = position_;

        header.emptyStringIndex = emptyStringIndex_;
        header.defaultPositionIndex = defaultPositionIndex_;
        header.defaultScaleIndex = defaultScaleIndex_;
        header.defaultSkewIndex = defaultSkewIndex_;
        header.defaultCollisionRectScaleIndex = defaultCollisionRectScaleIndex_;

        header.sourceIndex = sourceIndex_;

        // Calculate the size
        size_t size = aligned8MemorySize(sizeof(header));
        offsets.push_back(size);

        // Strings
        size += aligned8MemorySize(strings_.getMemoryUsage());
        offsets.push_back(size);

        // Atlas Subtextures
        size += aligned8MemorySize(atlasSubtextures_.getMemoryUsage());
        offsets.push_back(size);

        // Sub Texture Defs
        size += aligned8MemorySize(subTextureDefs_.getMemoryUsage());
        offsets.push_back(size);

        // Textures
        size += aligned8MemorySize(textures_.getMemoryUsage());
        offsets.push_back(size);

        // Text
        size += aligned8MemorySize(text_.getMemoryUsage());
        offsets.push_back(size);

        // Animation Sequences
        size += aligned8MemorySize(animationSequences_.getMemoryUsage());
        offsets.push_back(size);

        // Placements
        size += aligned8MemorySize(placements_.getMemoryUsage());
        offsets.push_back(size);

        // Masks
        size += aligned8MemorySize(masks_.getMemoryUsage());
        offsets.push_back(size);

        // Texture Masks
        size += aligned8MemorySize(textureMasks_.getMemoryUsage());
        offsets.push_back(size);

        // Buttons
        size += aligned8MemorySize(buttons_.getMemoryUsage());
        offsets.push_back(size);

        // Button States
        size += aligned8MemorySize(buttonStates_.getMemoryUsage());
        offsets.push_back(size);

        // External Packages
        size += aligned8MemorySize(externalPackages_.getMemoryUsage());
        offsets.push_back(size);

        // Auto Display Elements
        size += aligned8MemorySize(autoDisplayElements_.getMemoryUsage());
        offsets.push_back(size);

        // Rects
        size += aligned8MemorySize(rects_.getMemoryUsage());
        offsets.push_back(size);

        // Color Tranforms
        size += aligned8MemorySize(colorTransforms_.getMemoryUsage());
        offsets.push_back(size);

        // Color Matrices
        size += aligned8MemorySize(colorMatrices_.getMemoryUsage());
        offsets.push_back(size);

        // Vector2s
        size += aligned8MemorySize(vector2s_.getMemoryUsage());
        offsets.push_back(size);

        // Key Frame Indices
        size += aligned8MemorySize(keyframeIndices_.getMemoryUsage());
        offsets.push_back(size);

        // Bounds
        size += aligned8MemorySize(bounds_.getMemoryUsage());
        offsets.push_back(size);

        // Keyframes
        size += aligned8MemorySize(keyframes_.getMemoryUsage());
        offsets.push_back(size);

        // Channels
        size += aligned8MemorySize(channels_.getMemoryUsage());
        offsets.push_back(size);

        uint32_t sizeIndex = 0;
        buffer_ = new uint64_t[size / sizeof(uint64_t)];
        bufferSize_ = size;
        auto offset = offsets[sizeIndex];

        uint8_t *bytePtr = reinterpret_cast<uint8_t *>(buffer_);
        uint8_t *bufferPtr;

        // Strings
        uint8_t *src = reinterpret_cast<uint8_t *>(strings_.safeBaseAddress());
        if (src) {
            header.stringsOffset = static_cast<uint32_t>(offset);
            header.stringsSize = strings_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, strings_.getMemoryUsage());
        }
        ++sizeIndex;

        // Atlas Subtextures
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(atlasSubtextures_.safeBaseAddress());
        if (src) {
            header.atlasSubtexturesOffset = static_cast<uint32_t>(offset);
            header.atlasSubtexturesSize = atlasSubtextures_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, atlasSubtextures_.getMemoryUsage());
        }
        ++sizeIndex;

        // Sub Texture Defs
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(subTextureDefs_.safeBaseAddress());
        if (src) {
            header.subTextureDefsOffset = static_cast<uint32_t>(offset);
            header.subTextureDefsSize = subTextureDefs_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, subTextureDefs_.getMemoryUsage());
        }
        ++sizeIndex;

        // Textures
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(textures_.safeBaseAddress());
        if (src) {
            header.texturesOffset = static_cast<uint32_t>(offset);
            header.texturesSize = textures_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, textures_.getMemoryUsage());
        }
        ++sizeIndex;

        // Text
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(text_.safeBaseAddress());
        if (src) {
            header.textOffset = static_cast<uint32_t>(offset);
            header.textSize = text_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, text_.getMemoryUsage());
        }
        ++sizeIndex;

        // Animation Sequences
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(animationSequences_.safeBaseAddress());
        if (src) {
            header.animationSequencesOffset = static_cast<uint32_t>(offset);
            header.animationSequencesSize = animationSequences_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, animationSequences_.getMemoryUsage());
        }
        ++sizeIndex;

        // Placements
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(placements_.safeBaseAddress());
        if (src) {
            header.placementsOffset = static_cast<uint32_t>(offset);
            header.placementsSize = placements_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, placements_.getMemoryUsage());
        }
        ++sizeIndex;

        // Masks
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(masks_.safeBaseAddress());
        if (src) {
            header.masksOffset = static_cast<uint32_t>(offset);
            header.masksSize = masks_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, masks_.getMemoryUsage());
        }
        ++sizeIndex;

        // Texture Masks
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(textureMasks_.safeBaseAddress());
        if (src) {
            header.textureMasksOffset = static_cast<uint32_t>(offset);
            header.textureMasksSize = textureMasks_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, textureMasks_.getMemoryUsage());
        }
        ++sizeIndex;

        // Buttons
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(buttons_.safeBaseAddress());
        if (src) {
            header.buttonsOffset = static_cast<uint32_t>(offset);
            header.buttonsSize = buttons_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, buttons_.getMemoryUsage());
        }
        ++sizeIndex;

        // Button States
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(buttonStates_.safeBaseAddress());
        if (src) {
            header.buttonStatesOffset = static_cast<uint32_t>(offset);
            header.buttonStatesSize = buttonStates_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, buttonStates_.getMemoryUsage());
        }
        ++sizeIndex;

        // External Packages
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(externalPackages_.safeBaseAddress());
        if (src) {
            header.externalPackagesOffset = static_cast<uint32_t>(offset);
            header.externalPackagesSize = externalPackages_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, externalPackages_.getMemoryUsage());
        }
        ++sizeIndex;

        // Auto Display Elements
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(autoDisplayElements_.safeBaseAddress());
        if (src) {
            header.autoDisplayElementsOffset = static_cast<uint32_t>(offset);
            header.autoDisplayElementsSize = autoDisplayElements_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, autoDisplayElements_.getMemoryUsage());
        }
        ++sizeIndex;

        // Rects
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(rects_.safeBaseAddress());
        if (src) {
            header.rectsOffset = static_cast<uint32_t>(offset);
            header.rectsSize = rects_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, rects_.getMemoryUsage());
        }
        ++sizeIndex;

        // Color Tranforms
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(colorTransforms_.safeBaseAddress());
        if (src) {
            header.colorTransformsOffset = static_cast<uint32_t>(offset);
            header.colorTransformsSize = colorTransforms_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, colorTransforms_.getMemoryUsage());
        }
        ++sizeIndex;

        // Color Matrices
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(colorMatrices_.safeBaseAddress());
        if (src) {
            header.colorMatricesOffset = static_cast<uint32_t>(offset);
            header.colorMatricesSize = colorMatrices_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, colorMatrices_.getMemoryUsage());
        }
        ++sizeIndex;

        // Vector2s
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(vector2s_.safeBaseAddress());
        if (src) {
            header.vector2sOffset = static_cast<uint32_t>(offset);
            header.vector2sSize = vector2s_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, vector2s_.getMemoryUsage());
        }
        ++sizeIndex;

        // Key Frame Indices
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(keyframeIndices_.safeBaseAddress());
        if (src) {
            header.keyframeIndicesOffset = static_cast<uint32_t>(offset);
            header.keyframeIndicesSize = keyframeIndices_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, keyframeIndices_.getMemoryUsage());
        }
        ++sizeIndex;

        // Bounds
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(bounds_.safeBaseAddress());
        if (src) {
            header.boundsOffset = static_cast<uint32_t>(offset);
            header.boundsSize = bounds_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, bounds_.getMemoryUsage());
        }
        ++sizeIndex;

        // Keyframes
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(keyframes_.safeBaseAddress());
        if (src) {
            header.keyframesOffset = static_cast<uint32_t>(offset);
            header.keyframesSize = keyframes_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, keyframes_.getMemoryUsage());
        }
        ++sizeIndex;

        // Channels
        offset = offsets[sizeIndex];
        src = reinterpret_cast<uint8_t *>(channels_.safeBaseAddress());
        if (src) {
            header.channelsOffset = static_cast<uint32_t>(offset);
            header.channelsSize = channels_.size();
            bufferPtr = reinterpret_cast<uint8_t *>(&bytePtr[offset]);
            memcpy(bufferPtr, src, channels_.getMemoryUsage());
        }
        ++sizeIndex;

        // Header
        bufferPtr = reinterpret_cast<uint8_t *>(buffer_);
        memcpy(bufferPtr, &header, sizeof(header));
    }

    if (filename.size()) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (fp) {
            fwrite(buffer_, 1, bufferSize_, fp);
            fclose(fp);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//  BGE::ScenePackage
//
////////////////////////////////////////////////////////////////////////////////

BGE::ScenePackage::ScenePackage() : NamedObject(), status_(ScenePackageStatus::Invalid), frameRate_(0), pixelsPerPoint_(0), width_(0), height_(0), emptyStringIndex_(NullPtrIndex), defaultPositionIndex_(NullPtrIndex), defaultScaleIndex_(NullPtrIndex), defaultSkewIndex_(NullPtrIndex), defaultCollisionRectScaleIndex_(NullPtrIndex), sourceIndex_(NullPtrIndex), fontsLoaded_(false), texturesLoaded_(false), hasExternal_(false), memoryUsage_(0) {
    position_ = Vector2{{0, 0}};
}

BGE::ScenePackage::ScenePackage(ObjectId sceneId) : NamedObject(sceneId), status_(ScenePackageStatus::Invalid), frameRate_(0), pixelsPerPoint_(0), width_(0), height_(0), emptyStringIndex_(NullPtrIndex), defaultPositionIndex_(NullPtrIndex), defaultScaleIndex_(NullPtrIndex), defaultSkewIndex_(NullPtrIndex), defaultCollisionRectScaleIndex_(NullPtrIndex), sourceIndex_(NullPtrIndex), fontsLoaded_(false), texturesLoaded_(false), hasExternal_(false), memoryUsage_(0) {
    position_ = Vector2{{0, 0}};
}

void BGE::ScenePackage::initialize(ScenePackageHandle handle, std::string name) {
    handle_ = handle;
    setName(name);
    
    status_ = ScenePackageStatus::Invalid;
    fontsLoaded_ = false;
    texturesLoaded_ = false;
    hasExternal_ = false;

    memoryUsage_ = 0;
    
    baseDirectory_.type = FileUtilities::PathType::builtin;
    baseDirectory_.subpath.clear();
}

void BGE::ScenePackage::destroy() {
    source_ = "";

    status_ = ScenePackageStatus::Invalid;
    
    fontsLoaded_ = false;
    texturesLoaded_ = false;
    hasExternal_ = false;
    
    memoryUsage_ = 0;
    
    strings_.clear();
    atlasSubtextures_.clear();
    subTextureDefs_.clear();
    textures_.clear();
    text_.clear();
    animationSequences_.clear();
    placements_.clear();
    masks_.clear();
    textureMasks_.clear();
    buttons_.clear();
    buttonStates_.clear();
    externalPackages_.clear();
    autoDisplayElements_.clear();
    
    rects_.clear();
    colorTransforms_.clear();
    colorMatrices_.clear();
    vector2s_.clear();
    keyframeIndices_.clear();

    bounds_.clear();
    keyframes_.clear();
    channels_.clear();

    subTextures_.clear();
    
    textureQueue_.clear();
    
    fontQueue_.clear();

    textureNames_.clear();
    textureIndices_.clear();
    textureRefs_.clear();
    
    textNames_.clear();
    textIndices_.clear();
    textRefs_.clear();
    
    boundsRefs_.clear();
    
    animSeqNames_.clear();
    animSeqIndices_.clear();
    animSeqRefs_.clear();
    
    animChannelNames_.clear();
    animChannelIndices_.clear();
    animChannelRefs_.clear();
    
    animKeyframeNames_.clear();
    animKeyframeIndices_.clear();
    animKeyframeRefs_.clear();
    
    animChannelKeyframes_.clear();
    
    placementNames_.clear();
    placementIndices_.clear();
    placementRefs_.clear();
    
    maskNames_.clear();
    maskIndices_.clear();
    maskRefs_.clear();
    
    textureMaskNames_.clear();
    textureMaskIndices_.clear();
    textureMaskRefs_.clear();
    
    buttonStateRefs_.clear();
    
    buttonNames_.clear();
    buttonIndices_.clear();
    buttonRefs_.clear();
    
    externalPackageNames_.clear();
    externalPackageIndices_.clear();
    externalPackageRefs_.clear();
    
    autoDisplayElementRefs_.clear();
    
    referenceTypes_.clear();

    // Destroy all fonts
    auto fontService = Game::getInstance()->getFontService();
    
    for (auto const &font : loadedFonts_) {
        fontService->removeFont(handle_, font);
    }
    
    loadedFonts_.clear();
    
    // Destroy all texture atlases
    auto textureService = Game::getInstance()->getTextureService();
    
    for (auto const &atlas : loadedTextureAtlases_) {
        textureService->removeTextureAtlas(handle_, atlas);
    }
    
    loadedTextureAtlases_.clear();
    
    // Destroy all textures
    for (auto const &texture : loadedTextures_) {
        textureService->removeTexture(handle_, texture);
        
    }
    
    loadedTextures_.clear();
    
    // Resetting of the handle must be done at the end
    handle_ = ScenePackageHandle();
}

size_t BGE::ScenePackage::getMemoryUsage() const {
    return memoryUsage_;
}

void BGE::ScenePackage::computeMemoryUsage() {
    memoryUsage_ = 0;
    memoryUsage_ += strings_.getMemoryUsage();
    memoryUsage_ += atlasSubtextures_.getMemoryUsage();
    memoryUsage_ += subTextureDefs_.getMemoryUsage();
    memoryUsage_ += textures_.getMemoryUsage();
    memoryUsage_ += text_.getMemoryUsage();
    memoryUsage_ += animationSequences_.getMemoryUsage();
    memoryUsage_ += placements_.getMemoryUsage();
    memoryUsage_ += masks_.getMemoryUsage();
    memoryUsage_ += textureMasks_.getMemoryUsage();
    memoryUsage_ += buttons_.getMemoryUsage();
    memoryUsage_ += buttonStates_.getMemoryUsage();
    memoryUsage_ += externalPackages_.getMemoryUsage();
    memoryUsage_ += autoDisplayElements_.getMemoryUsage();
    memoryUsage_ += rects_.getMemoryUsage();
    memoryUsage_ += colorTransforms_.getMemoryUsage();
    memoryUsage_ += colorMatrices_.getMemoryUsage();
    memoryUsage_ += vector2s_.getMemoryUsage();
    memoryUsage_ += keyframeIndices_.getMemoryUsage();
    memoryUsage_ += bounds_.getMemoryUsage();
    memoryUsage_ += keyframes_.getMemoryUsage();
    memoryUsage_ += channels_.getMemoryUsage();
    memoryUsage_ += textureNames_.getMemoryUsage();
    memoryUsage_ += textureIndices_.getMemoryUsage();
    memoryUsage_ += textureRefs_.getMemoryUsage();
    memoryUsage_ += textNames_.getMemoryUsage();
    memoryUsage_ += textIndices_.getMemoryUsage();
    memoryUsage_ += textRefs_.getMemoryUsage();
    memoryUsage_ += boundsRefs_.getMemoryUsage();
    memoryUsage_ += animSeqNames_.getMemoryUsage();
    memoryUsage_ += animSeqIndices_.getMemoryUsage();
    memoryUsage_ += animSeqRefs_.getMemoryUsage();
    memoryUsage_ += animChannelNames_.getMemoryUsage();
    memoryUsage_ += animChannelIndices_.getMemoryUsage();
    memoryUsage_ += animChannelRefs_.getMemoryUsage();
    memoryUsage_ += animKeyframeNames_.getMemoryUsage();
    memoryUsage_ += animKeyframeIndices_.getMemoryUsage();
    memoryUsage_ += animKeyframeRefs_.getMemoryUsage();
    memoryUsage_ += animChannelKeyframes_.getMemoryUsage();
    memoryUsage_ += placementNames_.getMemoryUsage();
    memoryUsage_ += placementIndices_.getMemoryUsage();
    memoryUsage_ += placementRefs_.getMemoryUsage();
    memoryUsage_ += maskNames_.getMemoryUsage();
    memoryUsage_ += maskIndices_.getMemoryUsage();
    memoryUsage_ += maskRefs_.getMemoryUsage();
    memoryUsage_ += textureMaskNames_.getMemoryUsage();
    memoryUsage_ += textureMaskIndices_.getMemoryUsage();
    memoryUsage_ += textureMaskRefs_.getMemoryUsage();
    memoryUsage_ += buttonStateRefs_.getMemoryUsage();
    memoryUsage_ += buttonNames_.getMemoryUsage();
    memoryUsage_ += buttonIndices_.getMemoryUsage();
    memoryUsage_ += buttonRefs_.getMemoryUsage();
    memoryUsage_ += externalPackageNames_.getMemoryUsage();
    memoryUsage_ += externalPackageIndices_.getMemoryUsage();
    memoryUsage_ += externalPackageRefs_.getMemoryUsage();
    memoryUsage_ += autoDisplayElementRefs_.getMemoryUsage();
}

void BGE::ScenePackage::outputResourceBreakdown(uint32_t numTabs) const {
    Game::outputValue(numTabs, "ScenePackage (%s):\n", getName().c_str());

    numTabs++;
    
    Game::outputValue(numTabs, "Num Strings: %d\n", strings_.size());
    Game::outputValue(numTabs, "Num TextureAtlasSubTextureDefIntermediate: %d\n", atlasSubtextures_.size());
    Game::outputValue(numTabs, "Num SubTextureDefIntermediate: %d\n", subTextureDefs_.size());
    Game::outputValue(numTabs, "Num TextureReferenceIntermediate: %d\n", textures_.size());
    Game::outputValue(numTabs, "Num TextReferenceIntermediate: %d\n", text_.size());
    Game::outputValue(numTabs, "Num AnimationSequenceReferenceIntermediate: %d\n", animationSequences_.size());
    Game::outputValue(numTabs, "Num PlacementIntermediate: %d\n", placements_.size());
    Game::outputValue(numTabs, "Num MaskIntermediate: %d\n", masks_.size());
    Game::outputValue(numTabs, "Num TextureMaskIntermediate: %d\n", textureMasks_.size());
    Game::outputValue(numTabs, "Num ButtonIntermediate: %d\n", buttons_.size());
    Game::outputValue(numTabs, "Num ButtonStateIntermediate: %d\n", buttonStates_.size());
    Game::outputValue(numTabs, "Num ExternalPackageIntermediate: %d\n", externalPackages_.size());
    Game::outputValue(numTabs, "Num AutoDisplayElementIntermediate: %d\n", autoDisplayElements_.size());
    Game::outputValue(numTabs, "Num Rect: %d\n", rects_.size());
    Game::outputValue(numTabs, "Num ColorTransform: %d\n", colorTransforms_.size());
    Game::outputValue(numTabs, "Num ColorMatrix: %d\n", colorMatrices_.size());
    Game::outputValue(numTabs, "Num Vector2: %d\n", vector2s_.size());
    Game::outputValue(numTabs, "Num Keyframe Indicies: %d\n", keyframeIndices_.size());
    Game::outputValue(numTabs, "Num BoundsReferenceIntermediate: %d\n", bounds_.size());
    Game::outputValue(numTabs, "Num AnimationKeyframeReferenceIntermediate: %d\n", keyframes_.size());
    Game::outputValue(numTabs, "Num AnimationChannelReferenceIntermediate: %d\n", channels_.size());
    
    Game::outputValue(numTabs, "Num Texture Names: %d\n", textureNames_.size());
    Game::outputValue(numTabs, "Num Texture Indices: %d\n", textureIndices_.size());
    Game::outputValue(numTabs, "Num TextureReference: %d\n", textureRefs_.size());
    Game::outputValue(numTabs, "Num Text Names: %d\n", textNames_.size());
    Game::outputValue(numTabs, "Num Text Indices: %d\n", textIndices_.size());
    Game::outputValue(numTabs, "Num TextReference: %d\n", textRefs_.size());
    Game::outputValue(numTabs, "Num BoundsReference: %d\n", boundsRefs_.size());
    Game::outputValue(numTabs, "Num AnimationSequence Names: %d\n", animSeqNames_.size());
    Game::outputValue(numTabs, "Num AnimationSequence Indices: %d\n", animSeqIndices_.size());
    Game::outputValue(numTabs, "Num AnimationSequenceReference: %d\n", animSeqRefs_.size());
    Game::outputValue(numTabs, "Num AnimationChannel Names: %d\n", animChannelNames_.size());
    Game::outputValue(numTabs, "Num AnimationChannel Indices: %d\n", animChannelIndices_.size());
    Game::outputValue(numTabs, "Num AnimationChannelReference: %d\n", animChannelRefs_.size());
    Game::outputValue(numTabs, "Num AnimationKeyframe Names: %d\n", animKeyframeNames_.size());
    Game::outputValue(numTabs, "Num AnimationKeyframe Indices: %d\n", animKeyframeIndices_.size());
    Game::outputValue(numTabs, "Num AnimationKeyframeReference: %d\n", animKeyframeRefs_.size());
    Game::outputValue(numTabs, "Num AnimationKeyframeReference *: %d\n", animChannelKeyframes_.size());
    Game::outputValue(numTabs, "Num Placement Names: %d\n", placementNames_.size());
    Game::outputValue(numTabs, "Num Placement Indices: %d\n", placementIndices_.size());
    Game::outputValue(numTabs, "Num PlacementReference: %d\n", placementRefs_.size());
    Game::outputValue(numTabs, "Num Mask Names: %d\n", maskNames_.size());
    Game::outputValue(numTabs, "Num Mask Indices: %d\n", maskIndices_.size());
    Game::outputValue(numTabs, "Num MaskReference: %d\n", maskRefs_.size());
    Game::outputValue(numTabs, "Num TextureMask Names: %d\n", textureMaskNames_.size());
    Game::outputValue(numTabs, "Num TextureMask Indices: %d\n", textureMaskIndices_.size());
    Game::outputValue(numTabs, "Num TextureMaskReference: %d\n", textureMaskRefs_.size());
    Game::outputValue(numTabs, "Num ButtonStateReference: %d\n", buttonStateRefs_.size());
    Game::outputValue(numTabs, "Num Button Names: %d\n", buttonNames_.size());
    Game::outputValue(numTabs, "Num Button Indices: %d\n", buttonIndices_.size());
    Game::outputValue(numTabs, "Num ButtonReference: %d\n", buttonRefs_.size());
    Game::outputValue(numTabs, "Num ExternalPackage Names: %d\n", externalPackageNames_.size());
    Game::outputValue(numTabs, "Num ExternalPackage Indices: %d\n", externalPackageIndices_.size());
    Game::outputValue(numTabs, "Num ExternalPackageReference: %d\n", externalPackageRefs_.size());
    Game::outputValue(numTabs, "Num AutoDisplayElementReference: %d\n", autoDisplayElementRefs_.size());
}

void BGE::ScenePackage::outputMemoryBreakdown(uint32_t numTabs) const {
    Game::outputValue(numTabs, "ScenePackage (%s):\n", getName().c_str());
    
    numTabs++;
    
    Game::outputValue(numTabs, "Strings: %zd bytes\n", strings_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureAtlasSubTextureDefIntermediate: %zd bytes\n", atlasSubtextures_.getMemoryUsage());
    Game::outputValue(numTabs, "SubTextureDefIntermediate: %zd bytes\n", subTextureDefs_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureReferenceIntermediate: %zd bytes\n", textures_.getMemoryUsage());
    Game::outputValue(numTabs, "TextReferenceIntermediate: %zd bytes\n", text_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationSequenceReferenceIntermediate: %zd bytes\n", animationSequences_.getMemoryUsage());
    Game::outputValue(numTabs, "PlacementIntermediate: %zd bytes\n", placements_.getMemoryUsage());
    Game::outputValue(numTabs, "MaskIntermediate: %zd bytes\n", masks_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureMaskIntermediate: %zd bytes\n", textureMasks_.getMemoryUsage());
    Game::outputValue(numTabs, "ButtonIntermediate: %zd bytes\n", buttons_.getMemoryUsage());
    Game::outputValue(numTabs, "ButtonStateIntermediate: %zd bytes\n", buttonStates_.getMemoryUsage());
    Game::outputValue(numTabs, "ExternalPackageIntermediate: %zd bytes\n", externalPackages_.getMemoryUsage());
    Game::outputValue(numTabs, "AutoDisplayElementIntermediate: %zd bytes\n", autoDisplayElements_.getMemoryUsage());
    Game::outputValue(numTabs, "Rect: %zd bytes\n", rects_.getMemoryUsage());
    Game::outputValue(numTabs, "ColorTransform: %zd bytes\n", colorTransforms_.getMemoryUsage());
    Game::outputValue(numTabs, "ColorMatrix: %zd bytes\n", colorMatrices_.getMemoryUsage());
    Game::outputValue(numTabs, "Vector2: %zd bytes\n", vector2s_.getMemoryUsage());
    Game::outputValue(numTabs, "Keyframe Indicies: %zd bytes\n", keyframeIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "BoundsReferenceIntermediate: %zd bytes\n", bounds_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationKeyframeReferenceIntermediate: %zd bytes\n", keyframes_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationChannelReferenceIntermediate: %zd bytes\n", channels_.getMemoryUsage());
    
    Game::outputValue(numTabs, "Texture Names: %zd bytes\n", textureNames_.getMemoryUsage());
    Game::outputValue(numTabs, "Texture Indices: %zd bytes\n", textureIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureReference: %zd bytes\n", textureRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "Text Names: %zd bytes\n", textNames_.getMemoryUsage());
    Game::outputValue(numTabs, "Text Indices: %zd bytes\n", textIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "TextReference: %zd bytes\n", textRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "BoundsReference: %zd bytes\n", boundsRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationSequence Names: %zd bytes\n", animSeqNames_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationSequence Indices: %zd bytes\n", animSeqIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationSequenceReference: %zd bytes\n", animSeqRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationChannel Names: %zd bytes\n", animChannelNames_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationChannel Indices: %zd bytes\n", animChannelIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationChannelReference: %zd bytes\n", animChannelRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationKeyframe Names: %zd bytes\n", animKeyframeNames_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationKeyframe Indices: %zd bytes\n", animKeyframeIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationKeyframeReference: %zd bytes\n", animKeyframeRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "AnimationKeyframeReference *: %zd bytes\n", animChannelKeyframes_.getMemoryUsage());
    Game::outputValue(numTabs, "Placement Names: %zd bytes\n", placementNames_.getMemoryUsage());
    Game::outputValue(numTabs, "Placement Indices: %zd bytes\n", placementIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "PlacementReference: %zd bytes\n", placementRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "Mask Names: %zd bytes\n", maskNames_.getMemoryUsage());
    Game::outputValue(numTabs, "Mask Indices: %zd bytes\n", maskIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "MaskReference: %zd bytes\n", maskRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureMask Names: %zd bytes\n", textureMaskNames_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureMask Indices: %zd bytes\n", textureMaskIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "TextureMaskReference: %zd bytes\n", textureMaskRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "ButtonStateReference: %zd bytes\n", buttonStateRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "Button Names: %zd bytes\n", buttonNames_.getMemoryUsage());
    Game::outputValue(numTabs, "Button Indices: %zd bytes\n", buttonIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "ButtonReference: %zd bytes\n", buttonRefs_.size());
    Game::outputValue(numTabs, "ExternalPackage Names: %zd bytes\n", externalPackageNames_.getMemoryUsage());
    Game::outputValue(numTabs, "ExternalPackage Indices: %zd bytes\n", externalPackageIndices_.getMemoryUsage());
    Game::outputValue(numTabs, "ExternalPackageReference: %zd bytes\n", externalPackageRefs_.getMemoryUsage());
    Game::outputValue(numTabs, "AutoDisplayElementReference: %zd bytes\n", autoDisplayElementRefs_.getMemoryUsage());
}

BGE::BaseDirectory BGE::ScenePackage::getBaseDirectory() const {
    return baseDirectory_;
}

void BGE::ScenePackage::setBaseDirectory(const BaseDirectory &baseDirectory) {
    baseDirectory_ = baseDirectory;
}

BGE::AutoDisplayElementReference *BGE::ScenePackage::getAutoDisplayList() const {
    return autoDisplayElementRefs_.baseAddress();
}

int32_t BGE::ScenePackage::getAutoDisplayListSize() const {
    return autoDisplayElementRefs_.getSize();
}

void BGE::ScenePackage::link() {
    if (status_ != ScenePackageStatus::Valid) {
        return;
    }

    // Link Sub Textures

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
        const char *resolvedName = strings_.addressOf(texRefInt->name);
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
        textRef->multiline = textRefInt->multiline;

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
        if (keyframeRefInt->skew != NullPtrIndex) {
            keyframeRef->skew = keyframeRefInt->skew + vector2s_.baseAddress();
        } else {
            keyframeRef->skew = nullptr;
        }
        if (keyframeRefInt->collisionRectScale != NullPtrIndex) {
            keyframeRef->collisionRectScale = keyframeRefInt->collisionRectScale + vector2s_.baseAddress();
        } else {
            keyframeRef->collisionRectScale = nullptr;
        }
        keyframeRef->rotation = keyframeRefInt->rotation;
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
        int32_t    *keyframes = keyframeIndices_.addressOf(animChannelIntRef->keyframesStart);
        animChannelRef->name = resolvedName;
        animChannelRef->reference = reference;
        animChannelRef->referenceType = animChannelIntRef->referenceType;
        // Build the links related to keyframes
        animChannelRef->numKeyframes = (int32_t) animChannelIntRef->numKeyframes;
        animChannelRef->keyframes = animChannelKeyframes_.addressOf(keyframeIndex);

        for (uint32_t ki=0;ki<animChannelRef->numKeyframes;ki++) {
            animChannelRef->keyframes[ki] = animKeyframeRefs_.addressOf(keyframes[ki]);
        }

        animChannelRef->scenePackage = getHandle();

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

        animSeqRef->scenePackage = getHandle();

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
        button->scenePackage = getHandle();

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
        if (elemInt->skew != NullPtrIndex) {
            elem->skew = elemInt->skew + vector2s_.baseAddress();
        } else {
            elem->skew = nullptr;
        }
        if (elemInt->collisionRectScale != NullPtrIndex) {
            elem->collisionRectScale = elemInt->collisionRectScale + vector2s_.baseAddress();
        } else {
            elem->collisionRectScale = nullptr;
        }
        elem->rotation = elemInt->rotation;
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

    status_ = ScenePackageStatus::Linked;

    computeMemoryUsage();
}

void BGE::ScenePackage::sortChannels(AnimationChannelReferenceIntermediate *channels, int32_t size) {
    int32_t i, j;
    
    for (i=1;i<size;++i) {
        j = i;
        
        while (j > 0 && (channels[j].order < channels[j-1].order || (channels[j].order == channels[j-1].order && channels[j].startFrame >= channels[j-1].startFrame))) {
            auto temp = channels[j];
            channels[j] = channels[j-1];
            channels[j-1] = temp;
            --j;
        }
    }
}

void BGE::ScenePackage::create(const uint64_t *buffer, size_t bufferSize, bool managed, std::function<void(ScenePackage *)> callback) {
    if (status_ < ScenePackageStatus::Valid) {
        format_ = std::make_shared<ScenePackageFormat>(buffer, bufferSize, managed);

        frameRate_ = format_->frameRate_;
        pixelsPerPoint_ = format_->pixelsPerPoint_;
        width_ = format_->width_;
        height_ = format_->height_;
        position_ = format_->position_;

        emptyStringIndex_ = format_->emptyStringIndex_;
        defaultPositionIndex_ = format_->defaultPositionIndex_;
        defaultScaleIndex_ = format_->defaultScaleIndex_;
        defaultSkewIndex_ = format_->defaultSkewIndex_;
        defaultCollisionRectScaleIndex_ = format_->defaultCollisionRectScaleIndex_;

        sourceIndex_ = format_->sourceIndex_;

        // Strings
        strings_ = format_->strings_;

        // Atlas Subtextures
        atlasSubtextures_ = format_->atlasSubtextures_;

        // Sub Texture Defs
        subTextureDefs_ = format_->subTextureDefs_;

        // Textures
        textures_ = format_->textures_;

        // Text
        text_ = format_->text_;

        // Animation Sequences
        animationSequences_ = format_->animationSequences_;

        // Placements
        placements_ = format_->placements_;

        // Masks
        masks_ = format_->masks_;

        // Texture Masks
        textureMasks_ = format_->textureMasks_;

        // Buttons
        buttons_ = format_->buttons_;

        // Button States
        buttonStates_ = format_->buttonStates_;

        // External Packages
        externalPackages_ = format_->externalPackages_;

        // Auto Display Elements
        autoDisplayElements_ = format_->autoDisplayElements_;

        // Rects
        rects_ = format_->rects_;

        // Color Tranforms
        colorTransforms_ = format_->colorTransforms_;

        // Color Matrices
        colorMatrices_ = format_->colorMatrices_;

        // Vector2s
        vector2s_ = format_->vector2s_;

        // Key Frame Indices
        keyframeIndices_ = format_->keyframeIndices_;

        // Bounds
        bounds_ = format_->bounds_;

        // Keyframes
        keyframes_ = format_->keyframes_;

        // Channels
        channels_ = format_->channels_;

        // Source
        source_ = strings_.addressOf(sourceIndex_);

        computeMemoryUsage();

        loadAllTextures([this, callback]() {
            if (callback) {
                callback(this);
            }
        });
    } else if (callback) {
        callback(this);
    }
}

void BGE::ScenePackage::create(const std::shared_ptr<rapidjson::Document> jsonDict, std::function<void(ScenePackage *)> callback) {
    if (status_ < ScenePackageStatus::Valid) {
        using namespace rapidjson;
        try {
            StringArrayBuilder stringBuilder ;
            ArrayBuilder<SubTextureDefIntermediate, SubTextureDefIntermediate> subTexDefBuilder;
            ArrayBuilder<TextureAtlasSubTextureDefIntermediate, TextureAtlasSubTextureDefIntermediate> texAtlasSubTexDefBuilder;
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
            ArrayBuilder<int32_t, int32_t> keyframeIndicesBuilder;

            UniqueArrayBuilder<Rect, Rect> rectBuilder;
            UniqueArrayBuilder<ColorTransform, ColorTransform> colorXformBuilder;
            UniqueArrayBuilder<ColorMatrix, ColorMatrix> colorMtxBuilder;
            UniqueArrayBuilder<Vector2, Vector2> vector2Builder;

            ArrayBuilder<BoundsReferenceIntermediate, BoundsReferenceIntermediate> boundsRefIntBuilder;
            ArrayBuilder<AnimationKeyframeReferenceIntermediate, AnimationKeyframeReferenceIntermediate> keyframeIntBuilder;
            ArrayBuilder<AnimationChannelReferenceIntermediate, AnimationChannelReferenceIntermediate> channelRefIntBuilder;

            std::vector<AnimationChannelReferenceIntermediateKeyframes> channelRefIntKeyframes;
            std::unordered_map<std::string, std::vector<SubTextureDef>> subtextureDefs;

            uint32_t    channelId = 0;

            Value subtextures;
            Value textures;

            if (jsonDict->HasMember(kScenePackageKeySubtextures)) {
                subtextures = (*jsonDict)[kScenePackageKeySubtextures].GetArray();
            } else {
                subtextures = Value(kArrayType);
            }
            if (jsonDict->HasMember(kScenePackageKeyTextures)) {
                textures = (*jsonDict)[kScenePackageKeyTextures].GetArray();;

            } else {
                textures = Value(kObjectType);
            }

            if (jsonDict->HasMember(kScenePackageKeyPixelsPerPoint)) {
                pixelsPerPoint_ = (*jsonDict)[kScenePackageKeyPixelsPerPoint].GetFloat();
            } else {
                pixelsPerPoint_ = 1.0;
            }

            source_ = (*jsonDict)[kScenePackageKeySource].GetString();
            frameRate_ = (*jsonDict)[kScenePackageKeyFrameRate].GetFloat();
            if (jsonDict->HasMember(kScenePackageKeyWidth)) {
                width_ = (*jsonDict)[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
            }
            if (jsonDict->HasMember(kScenePackageKeyHeight)) {
                height_ = (*jsonDict)[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
            }
            if (jsonDict->HasMember(kScenePackageKeyPosition)) {
                position_.x = (*jsonDict)[kScenePackageKeyPosition][kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                position_.y = (*jsonDict)[kScenePackageKeyPosition][kScenePackageKeyY].GetFloat() * pixelsPerPoint_;
            }

            emptyStringIndex_ = stringBuilder.add("");
            defaultPositionIndex_ = vector2Builder.add(Vector2{{0, 0}});
            defaultScaleIndex_ = vector2Builder.add(Vector2{{1, 1}});
            defaultSkewIndex_ = vector2Builder.add(Vector2{{0, 0}});
            defaultCollisionRectScaleIndex_ = vector2Builder.add(Vector2{{1, 1}});

            sourceIndex_ = stringBuilder.add(source_);

            // TextureReferenceIntermediate total count are all textures and subTextures
            texIntBuilder.resize((int32_t) (textures.Size() + subtextures.Size()));

            // Do subtextures first, since we will use this info to know if our textures are atlases

            // Subtextures
            for (rapidjson::SizeType index=0;index<subtextures.Size();index++) {
                Value subTexDict = subtextures[index].GetObject();
                TextureReferenceIntermediate *texRef = texIntBuilder.addressOf(index);
                SubTextureDef subTexDef{};

                subTexDef.name = subTexDict[kScenePackageKeyName].GetString();
                subTexDef.x = subTexDict[kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                subTexDef.y = subTexDict[kScenePackageKeyY].GetFloat() * pixelsPerPoint_;
                subTexDef.width = subTexDict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                subTexDef.height = subTexDict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
                subTexDef.rotated = subTexDict[kScenePackageKeyRotated].GetBool();
                subTexDef.font = false;
                std::string atlasName = subTexDict[kScenePackageKeyAtlas].GetString();
#if 0
                std::vector<SubTextureDef> &subTexDefs = subTextures_[atlasName];
#else
                std::vector<SubTextureDef> &subTexDefs = subtextureDefs[atlasName];
#endif

                subTexDefs.push_back(subTexDef);

                texRef->name = stringBuilder.add(subTexDef.name.c_str());
                texRef->atlasName = stringBuilder.add(atlasName);
                texRef->width = subTexDef.width;
                texRef->height = subTexDef.height;
                texRef->isAtlas = false;
            }

#if 0
            texAtlasSubTexDefBuilder.resize((int32_t)subTextures_.size());
#else
            //texAtlasSubTexDefBuilder.resize((int32_t)subtextureDefs.size());
#endif
            for (auto& it : subtextureDefs) {
                TextureAtlasSubTextureDefIntermediate atlasSubTexDefInt{};
                atlasSubTexDefInt.name = stringBuilder.add(it.first);
                atlasSubTexDefInt.subTextDefStart = subTexDefBuilder.size();
                auto& subtex = it.second;
                for (auto& st : subtex) {
                    SubTextureDefIntermediate subTexDefInt;
                    subTexDefInt.name = stringBuilder.add(st.name);
                    subTexDefInt.x = st.x;
                    subTexDefInt.y = st.y;
                    subTexDefInt.width = st.width;
                    subTexDefInt.height = st.height;
                    subTexDefInt.rotated = st.rotated;
                    subTexDefBuilder.add(subTexDefInt);
                    ++atlasSubTexDefInt.numSubTextures;
                }
                texAtlasSubTexDefBuilder.add(atlasSubTexDefInt);
            }

            for (rapidjson::SizeType index=0;index<textures.Size();index++) {
                Value texDict = textures[index].GetObject();
                TextureReferenceIntermediate *texRef = texIntBuilder.addressOf((int) (subtextures.Size() + index));

                // Add name
                const char* name = texDict[kScenePackageKeyName].GetString();
                const char *filename = texDict[kScenePackageKeyFilename].GetString();

                assert(name);

                if (name) {
                    texRef->name = stringBuilder.add(name);
                    texRef->filename = stringBuilder.add(filename);
                    texRef->atlasName = NullPtrIndex;
                    texRef->width = texDict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                    texRef->height = texDict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;

                    auto subTex = subtextureDefs.find(name);

                    if (subTex != subtextureDefs.end()) {
                        texRef->isAtlas = true;
                    } else {
                        texRef->isAtlas = false;
                    }
                }
            }

            // Text
            if (jsonDict->HasMember(kScenePackageKeyText)) {
                Value text = (*jsonDict)[kScenePackageKeyText].GetArray();

                textIntBuilder.resize((int32_t) text.Size());

                for (rapidjson::SizeType index=0;index<text.Size();index++) {
                    Value textDict = text[index].GetObject();
                    TextReferenceIntermediate *textRef = textIntBuilder.addressOf(index);

                    const char *name = textDict[kScenePackageKeyName].GetString();
                    const char *text = textDict[kScenePackageKeyText].GetString();

                    assert(name);

                    if (name) {
                        Value formatDict = textDict[kScenePackageKeyFormat].GetObject();

                        textRef->name = stringBuilder.add(name);
                        textRef->text = stringBuilder.add(text);
                        textRef->width = textDict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                        textRef->height = textDict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
                        textRef->leading = formatDict[kScenePackageKeyLeading].GetFloat() * pixelsPerPoint_;
                        if (textDict.HasMember(kScenePackageKeyMultiline)) {
                            textRef->multiline = textDict[kScenePackageKeyMultiline].GetBool();
                        } else  {
                            textRef->multiline = false;
                        }

                        if (textRef->multiline) {
#if DEBUG
                            printf("Multi-line text for %s\n", name);
#endif
                        }
                        std::string alignment = formatDict[kScenePackageKeyAlignment].GetString();

                        if (alignment == kScenePackageKeyLeft){
                            textRef->alignment = FontHorizontalAlignment::Left;
                        } else if (alignment == kScenePackageKeyRight){
                            textRef->alignment = FontHorizontalAlignment::Right;
                        } else {
                            textRef->alignment = FontHorizontalAlignment::Center;
                        }

                        uint32_t size = formatDict[kScenePackageKeyFontSize].GetFloat() * pixelsPerPoint_;
                        uint32_t color = formatDict[kScenePackageKeyFontColor].GetUint();
                        uint32_t blue = color & 0xff;
                        uint32_t green = color >> 8 & 0xff;
                        uint32_t red = color >> 16 & 0xff;
                        uint32_t alpha = 255;

                        textRef->size = size;
                        textRef->color.r = red / 255.;
                        textRef->color.g = green / 255.;
                        textRef->color.b = blue / 255.;
                        textRef->color.a = alpha / 255.;

                        const char* fontName = formatDict[kScenePackageKeyFontName].GetString();
                        textRef->fontName = stringBuilder.add(fontName);
                    }
                }
            }

            // Symbols
            if (jsonDict->HasMember(kScenePackageKeySymbols)) {
                auto symbols = (*jsonDict)[kScenePackageKeySymbols].GetArray();

                animSeqIntBuilder.resize((int32_t) symbols.Size());

                for (rapidjson::SizeType index=0;index<symbols.Size();index++) {
                    auto symbolDict = symbols[index].GetObject();
                    AnimationSequenceReferenceIntermediate *animSeq = animSeqIntBuilder.addressOf(index);

                    const char *name = symbolDict[kScenePackageKeyName].GetString();

                    if (name) {
                        animSeq->name = stringBuilder.add(name);
                        animSeq->frameRate = frameRate_;
                        animSeq->bounds = (uint32_t) boundsRefIntBuilder.size();
                        animSeq->channels = (uint32_t) channelRefIntBuilder.size();

                        Value frames = symbolDict[kScenePackageKeyFrames].GetArray();
                        std::map<std::string, int32_t> channelIndex;

                        BoundsReference currBounds{};
                        BoundsReference newBounds{};
                        Rect currRect{};
                        Rect newRect{};

                        currBounds.startFrame = 0;
                        currBounds.totalFrames = 1;

                        animSeq->totalFrames = (uint32_t) frames.Size();

                        for (rapidjson::SizeType fi=0;fi<frames.Size();fi++) {
                            auto frameDict = frames[fi].GetObject();

                            if (fi == 0) {
                                currRect.x = frameDict[kScenePackageKeyBounds][kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                                currRect.y = frameDict[kScenePackageKeyBounds][kScenePackageKeyY].GetFloat() * pixelsPerPoint_;
                                currRect.w = frameDict[kScenePackageKeyBounds][kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                                currRect.h = frameDict[kScenePackageKeyBounds][kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
                            } else {
                                newBounds.startFrame = 0;
                                newBounds.totalFrames = 1;
                                newRect.x = frameDict[kScenePackageKeyBounds][kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                                newRect.y = frameDict[kScenePackageKeyBounds][kScenePackageKeyY].GetFloat() * pixelsPerPoint_;
                                newRect.w = frameDict[kScenePackageKeyBounds][kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                                newRect.h = frameDict[kScenePackageKeyBounds][kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;

                                if (newRect.x != currRect.x || newRect.y != currRect.y || newRect.w != currRect.w || newRect.h != currRect.h) {
                                    currRect = newRect;
                                    boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currRect)});
                                } else {
                                    currBounds.totalFrames++;
                                }
                            }

                            if (frameDict.HasMember(kScenePackageKeyChildren)) {
                                auto children = frameDict[kScenePackageKeyChildren].GetArray();

                                for (rapidjson::SizeType ci=0;ci<children.Size();ci++) {
                                    auto childDict = children[ci].GetObject();
                                    const char *childName = childDict[kScenePackageKeyName].GetString();

                                    assert(childName);

                                    if (childName) {
                                        auto channelIndexIt = channelIndex.find(childName);
                                        AnimationChannelReferenceIntermediate *channel;
                                        AnimationChannelReferenceIntermediateKeyframes *channelKeyframes;

                                        if (channelIndexIt != channelIndex.end()) {
                                            channel = channelRefIntBuilder.addressOf(channelIndexIt->second);
                                            channelKeyframes = &channelRefIntKeyframes[channelIndexIt->second];
                                            if (static_cast<int32_t>(ci) > channel->order) {
                                                channel->order = static_cast<int32_t>(ci);
                                            }
                                        } else {
                                            AnimationChannelReferenceIntermediate newChannel{};
                                            AnimationChannelReferenceIntermediateKeyframes newChannelKeyframes{};

                                            const char* reference = childDict[kScenePackageKeyReference].GetString();
                                            const char *type = childDict[kScenePackageKeyReferenceType].GetString();

                                            assert(reference && type);

                                            newChannel.chanId = channelId;
                                            newChannel.name = stringBuilder.add(childName);
                                            newChannel.reference = stringBuilder.add(reference);
                                            newChannel.startFrame = static_cast<int32_t>(fi);
                                            newChannel.order =  static_cast<int32_t>(ci);

                                            newChannelKeyframes.chanId = channelId;

                                            GfxReferenceType value = referenceTypeForString(type);

                                            newChannel.referenceType = value;

                                            auto idx = channelRefIntBuilder.size();
                                            channelIndex[childName] = idx;
                                            channelRefIntBuilder.add(newChannel);
#ifdef OBSOLETE
                                            channel = channelRefIntBuilder.addressOf(idx);
#endif
                                            channelRefIntKeyframes.push_back(newChannelKeyframes);
                                            channelKeyframes = &channelRefIntKeyframes[idx];

                                            ++channelId;
                                        }

                                        AnimationKeyframeReferenceIntermediate keyframe{};
                                        Rect boundsBacking{};
                                        Rect *bounds = &boundsBacking;
                                        ColorTransform colorTransformBacking{};
                                        ColorTransform *colorTransform = &colorTransformBacking;
                                        ColorMatrix colorMatrixBacking{};
                                        ColorMatrix *colorMatrix;
                                        Vector2 scaleBacking{};
                                        Vector2 *scale = &scaleBacking;
                                        Vector2 skewBacking{};
                                        Vector2 *skew = &skewBacking;
                                        Vector2 collisionRectScaleBacking{};
                                        Vector2 *collisionRectScale = &collisionRectScaleBacking;
                                        Vector2 positionBacking{};
                                        Vector2 *position = &positionBacking;

                                        keyframe.startFrame = (uint32_t)fi;
                                        keyframe.totalFrames = 1;
                                        keyframe.order = (uint32_t)ci;
                                        keyframe.flags = childDict[kScenePackageKeyFlags].GetUint();
                                        if (childDict.HasMember(kScenePackageKeyFrame)) {
                                            // referenceType = sprite will not have frame
                                            keyframe.frame = childDict[kScenePackageKeyFrame].GetUint() - 1;
                                        } else {
                                            keyframe.frame = 0;
                                        }

                                        // Bounds
                                        auto boundsDict = childDict[kScenePackageKeyBounds].GetObject();
                                        boundsBacking.x = boundsDict[kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                                        boundsBacking.y = boundsDict[kScenePackageKeyY].GetFloat() * pixelsPerPoint_;
                                        boundsBacking.w = boundsDict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                                        boundsBacking.h = boundsDict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;

                                        // Color
                                        if (childDict.HasMember(kScenePackageKeyColor)) {
                                            auto colorDict = childDict[kScenePackageKeyColor].GetObject();
                                            colorTransformBacking.multiplier.r = colorDict[kScenePackageKeyRedMultiplier].GetFloat();
                                            colorTransformBacking.multiplier.g = colorDict[kScenePackageKeyGreenMultiplier].GetFloat();
                                            colorTransformBacking.multiplier.b = colorDict[kScenePackageKeyBlueMultiplier].GetFloat();
                                            colorTransformBacking.multiplier.a = colorDict[kScenePackageKeyAlphaMultiplier].GetFloat();
                                            colorTransformBacking.offset.r = colorDict[kScenePackageKeyRedOffset].GetFloat() / 255.0;
                                            colorTransformBacking.offset.g = colorDict[kScenePackageKeyGreenOffset].GetFloat() / 255.0;
                                            colorTransformBacking.offset.b = colorDict[kScenePackageKeyBlueOffset].GetFloat() / 255.0;
                                            colorTransformBacking.offset.a = colorDict[kScenePackageKeyAlphaOffset].GetFloat() / 255.0;
                                            colorTransformBacking.color = colorDict[kScenePackageKeyColor].GetFloat();
                                        } else {
                                            colorTransform = nullptr;
                                        }

                                        // Filters
                                        colorMatrix = nullptr;

                                        if (childDict.HasMember(kScenePackageKeyFilters)) {
                                            auto filters = childDict[kScenePackageKeyFilters].GetArray();
                                            for (auto filter=filters.Begin();filter!=filters.End();++filter) {
                                                auto filterDict = filter->GetObject();
                                                if (filterDict.HasMember(kScenePackageKeyColorMatrixFilter)) {
                                                    auto colorMtxDict = filterDict[kScenePackageKeyColorMatrixFilter].GetObject();
                                                    auto rawMatrix = colorMtxDict[kScenePackageKeyMatrix].GetArray();
#if 0
                                                    for (NSInteger cmi=0;cmi<rawMatrix.count;cmi++) {
                                                        colorMatrixBacking.c[cmi] = [rawMatrix[cmi] floatValue];
                                                    }
#endif

                                                    float divisor = 1;
                                                    float divisor1 = 255.0;

                                                    colorMatrixBacking.matrix.m[0] = rawMatrix[0].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[1] = rawMatrix[1].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[2] = rawMatrix[2].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[3] = rawMatrix[3].GetFloat() / divisor;

                                                    colorMatrixBacking.matrix.m[4] = rawMatrix[5].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[5] = rawMatrix[6].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[6] = rawMatrix[7].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[7] = rawMatrix[8].GetFloat() / divisor;

                                                    colorMatrixBacking.matrix.m[8] = rawMatrix[10].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[9] = rawMatrix[11].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[10] = rawMatrix[12].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[11] = rawMatrix[13].GetFloat() / divisor;

                                                    colorMatrixBacking.matrix.m[12] = rawMatrix[15].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[13] = rawMatrix[16].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[14] = rawMatrix[17].GetFloat() / divisor;
                                                    colorMatrixBacking.matrix.m[15] = rawMatrix[18].GetFloat() / divisor;

                                                    colorMatrixBacking.offset.v[0] = rawMatrix[4].GetFloat() / divisor1;
                                                    colorMatrixBacking.offset.v[1] = rawMatrix[9].GetFloat() / divisor1;
                                                    colorMatrixBacking.offset.v[2] = rawMatrix[14].GetFloat() / divisor1;
                                                    colorMatrixBacking.offset.v[3] = rawMatrix[19].GetFloat() / divisor1;

                                                    colorMatrix = &colorMatrixBacking;

                                                }
                                            }
                                        }

                                        // Position
                                        if (childDict.HasMember(kScenePackageKeyPosition)) {
                                            auto dict = childDict[kScenePackageKeyPosition].GetObject();
                                            positionBacking.x = dict[kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                                            positionBacking.y = dict[kScenePackageKeyY].GetFloat() * pixelsPerPoint_;

                                            keyframe.position = (int32_t) vector2Builder.add(positionBacking);
                                        } else {
                                            position = nullptr;
                                        }

                                        // Scale
                                        if (childDict.HasMember(kScenePackageKeyScale)) {
                                            auto dict = childDict[kScenePackageKeyScale].GetObject();
                                            scaleBacking.x = dict[kScenePackageKeyX].GetFloat();
                                            scaleBacking.y = dict[kScenePackageKeyY].GetFloat();
                                        } else {
                                            scale = nullptr;
                                        }

                                        // Rotation
                                        if (childDict.HasMember(kScenePackageKeyRotation)) {
                                            keyframe.rotation = childDict[kScenePackageKeyRotation].GetFloat();
                                        } else {
                                            keyframe.rotation = 0;
                                        }

                                        // Skew
                                        if (childDict.HasMember(kScenePackageKeySkew)) {
                                            auto dict = childDict[kScenePackageKeySkew].GetObject();
                                            skewBacking.x = dict[kScenePackageKeyX].GetFloat();
                                            skewBacking.y = dict[kScenePackageKeyY].GetFloat();
                                        } else {
                                            skew = nullptr;
                                        }

                                        if (childDict.HasMember(kScenePackageKeyCollRectScale)) {
                                            auto dict = childDict[kScenePackageKeyCollRectScale].GetObject();
                                            collisionRectScaleBacking.x = dict[kScenePackageKeyX].GetFloat();
                                            collisionRectScaleBacking.y = dict[kScenePackageKeyY].GetFloat();
                                        } else {
                                            collisionRectScale = nullptr;
                                        }

                                        // TODO: Now build matrix

                                        // Now determine if this Keyframe is a new Keyframe
                                        bool isNew = false;
                                        AnimationKeyframeReferenceIntermediate *currKeyframe = nullptr;
                                        int32_t keyframeIndex;

                                        if (channelKeyframes->keyframes.size() == 0) {
                                            isNew = true;
                                        } else {
                                            keyframeIndex = channelKeyframes->keyframes[channelKeyframes->keyframes.size() - 1];
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
                                                Vector2 *currSkew;
                                                Vector2 *currCollisionRectScale;

                                                currBounds = rectBuilder.safeAddressOf(currKeyframe->bounds);
                                                currColorTransform = colorXformBuilder.safeAddressOf(currKeyframe->colorTransform);
                                                currColorMatrix = colorMtxBuilder.safeAddressOf(currKeyframe->colorMatrix);
                                                currPosition = vector2Builder.safeAddressOf(currKeyframe->position);
                                                currScale = vector2Builder.safeAddressOf(currKeyframe->scale);
                                                currSkew = vector2Builder.safeAddressOf(currKeyframe->skew);
                                                currCollisionRectScale = vector2Builder.safeAddressOf(currKeyframe->collisionRectScale);

                                                // Bounds the same?
                                                if ((currBounds != bounds) || ((currBounds && bounds) && (*currBounds != *bounds))) {
                                                    isNew = true;
                                                } else if ((currPosition != position) || ((currPosition && position) && (*currPosition != *position))) {
                                                    isNew = true;
                                                } else if ((currScale != scale) || ((currScale && scale) && (*currScale != *scale))) {
                                                    isNew = true;
                                                } else if ((currSkew != skew) || ((currSkew && skew) && (*currSkew != *skew))) {
                                                    isNew = true;
                                                } else if ((currCollisionRectScale != collisionRectScale) || ((currCollisionRectScale && collisionRectScale) && (*currCollisionRectScale != *collisionRectScale))) {
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

                                            if (skew) {
                                                keyframe.skew = vector2Builder.add(*skew);
                                            } else {
                                                keyframe.skew = defaultSkewIndex_;
                                            }

                                            if (collisionRectScale) {
                                                keyframe.collisionRectScale = vector2Builder.add(*collisionRectScale);
                                            } else {
                                                keyframe.collisionRectScale = defaultCollisionRectScaleIndex_;
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
                                            channelKeyframes->keyframes.push_back(keyframeIndex);
                                        } else if (currKeyframe) {
                                            currKeyframe->totalFrames++;
                                        } else {
                                            assert(false);
                                        }
                                    }
                                }
                            }
                        }

                        boundsRefIntBuilder.add({currBounds.startFrame, currBounds.totalFrames, rectBuilder.add(currRect)});

                        animSeq->numBounds = (uint32_t) boundsRefIntBuilder.size() - (uint32_t) animSeq->bounds;
                        animSeq->numChannels = (uint32_t) channelRefIntBuilder.size() - animSeq->channels;

                        // Sort channels for proper draw order
                        sortChannels(channelRefIntBuilder.addressOf(animSeq->channels), animSeq->numChannels);
                    }
                }
            }

            // Build keyframe indices
            for (auto i=0;i<channelRefIntBuilder.size();++i) {
                AnimationChannelReferenceIntermediate *channel = channelRefIntBuilder.addressOf(i);
                for (size_t fi=0;fi<channelRefIntKeyframes.size();++fi) {
                    auto& chanKeyframe = channelRefIntKeyframes[fi];
                    if (channel->chanId == chanKeyframe.chanId) {
                        channel->numKeyframes = static_cast<decltype(channel->numKeyframes)>(chanKeyframe.keyframes.size());
                        channel->keyframesStart = keyframeIndicesBuilder.size();
                        for (auto f : chanKeyframe.keyframes) {
                            keyframeIndicesBuilder.add(f);
                        }
                        break;
                    }
                }
            }

            // Placements
            if (jsonDict->HasMember(kScenePackageKeyPlacements)) {
                auto placements = (*jsonDict)[kScenePackageKeyPlacements].GetArray();

                placementIntBuilder.resize((int32_t) placements.Size());

                for (rapidjson::SizeType index=0;index<placements.Size();index++) {
                    auto dict = placements[index].GetObject();
                    PlacementIntermediate *placement = placementIntBuilder.addressOf(index);
                    const char *name = dict[kScenePackageKeyName].GetString();

                    assert(name);

                    if (name) {
                        placement->name = stringBuilder.add(name);
                        placement->width = dict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                        placement->height = dict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
                    }
                }
            }

            // Buttons
            if (jsonDict->HasMember(kScenePackageKeyButtons)) {
                auto buttons = (*jsonDict)[kScenePackageKeyButtons].GetArray();
                int32_t numStates = 0;

                for (rapidjson::SizeType index=0;index<buttons.Size();index++) {
                    auto buttonDict = buttons[index].GetObject();
                    auto states = buttonDict[kScenePackageKeyStates].GetArray();

                    numStates += states.Size();
                }

                buttonIntBuilder.resize((int32_t) buttons.Size());
                buttonStateIntBuilder.resize(numStates);

                int32_t stateIndex = 0;

                for (rapidjson::SizeType index=0;index<buttons.Size();index++) {
                    auto buttonDict = buttons[index].GetObject();
                    ButtonIntermediate *button = buttonIntBuilder.addressOf(index);
                    const char *buttonName = buttonDict[kScenePackageKeyName].GetString();

                    assert(buttonName);

                    if (buttonName) {
                        auto states = buttonDict[kScenePackageKeyStates].GetArray();

                        button->name = stringBuilder.add(buttonName);
                        button->states = stateIndex;
                        button->numStates = (int32_t) states.Size();

                        for (auto it=states.Begin();it!=states.End();++it) {
                            auto state = it->GetObject();
                            ButtonStateIntermediate *buttonState = buttonStateIntBuilder.addressOf(stateIndex);
                            const char *stateName = state[kScenePackageKeyState].GetString();
                            const char *reference = state[kScenePackageKeyReference].GetString();
                            const char *type = state[kScenePackageKeyReferenceType].GetString();

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
            }

            // Masks
            if (jsonDict->HasMember(kScenePackageKeyMasks)) {
                auto masks = (*jsonDict)[kScenePackageKeyMasks].GetArray();

                maskIntBuilder.resize((int32_t) masks.Size());

                for (rapidjson::SizeType index=0;index<masks.Size();index++) {
                    auto dict = masks[index].GetObject();
                    MaskIntermediate *mask = maskIntBuilder.addressOf(index);
                    const char *name = dict[kScenePackageKeyName].GetString();

                    assert(name);

                    if (name) {
                        mask->name = stringBuilder.add(name);
                        mask->width = dict[kScenePackageKeyWidth].GetFloat() * pixelsPerPoint_;
                        mask->height = dict[kScenePackageKeyHeight].GetFloat() * pixelsPerPoint_;
                    }
                }
            }

            // Texture Masks
            if (jsonDict->HasMember(kScenePackageKeyTextureMasks)) {
                auto textureMasks = (*jsonDict)[kScenePackageKeyTextureMasks].GetArray();

                textureMaskIntBuilder.resize((int32_t) textureMasks.Size());

                for (rapidjson::SizeType index=0;index<textureMasks.Size();index++) {
                    auto dict = textureMasks[index].GetObject();
                    TextureMaskIntermediate *mask = textureMaskIntBuilder.addressOf(index);
                    const char *name = dict[kScenePackageKeyName].GetString();

                    assert(name);

                    if (name) {
                        mask->name = stringBuilder.add(name);
                    }
                }
            }

            // External References
            if (jsonDict->HasMember(kScenePackageKeyExternalReferences)) {
                auto externals = (*jsonDict)[kScenePackageKeyExternalReferences].GetArray();

                extPackageIntBuilder.resize((int32_t) externals.Size());

                for (rapidjson::SizeType index=0;index<externals.Size();index++) {
                    auto dict = externals[index].GetObject();
                    ExternalPackageIntermediate *external = extPackageIntBuilder.addressOf(index);
                    const char *name = dict[kScenePackageKeyName].GetString();
                    const char *externalPackage = dict[kScenePackageKeyExternalPackage].GetString();

                    assert(name);
                    assert(externalPackage);

                    external->name = stringBuilder.add(name);
                    external->externalPackage = stringBuilder.add(externalPackage);
                }
            }

            // Auto display list elements
            if (jsonDict->HasMember(kScenePackageKeyAutoDisplayList)) {
                auto autoDisplayElems = (*jsonDict)[kScenePackageKeyAutoDisplayList].GetArray();

                autoDisplayElemIntBuilder.resize((int32_t) autoDisplayElems.Size());

                for (rapidjson::SizeType index=0;index<autoDisplayElems.Size();index++) {
                    auto dict = autoDisplayElems[index].GetObject();
                    AutoDisplayElementIntermediate *elem = autoDisplayElemIntBuilder.addressOf(index);
                    const char *name = dict[kScenePackageKeyName].GetString();
                    const char *reference = dict[kScenePackageKeyReference].GetString();
                    const char *type = dict[kScenePackageKeyReferenceType].GetString();

                    assert(name);
                    assert(reference);
                    assert(type);

                    GfxReferenceType refType = referenceTypeForString(type);

                    elem->name = stringBuilder.add(name);
                    elem->reference = stringBuilder.add(reference);
                    elem->referenceType = refType;

                    Rect bounds;

                    bounds.x = dict[kScenePackageKeyBounds][kScenePackageKeyX].GetFloat();
                    bounds.y = dict[kScenePackageKeyBounds][kScenePackageKeyY].GetFloat();
                    bounds.w = dict[kScenePackageKeyBounds][kScenePackageKeyWidth].GetFloat();
                    bounds.h = dict[kScenePackageKeyBounds][kScenePackageKeyHeight].GetFloat();

                    elem->bounds = rectBuilder.add(bounds);

                    if (dict.HasMember(kScenePackageKeyPosition)) {
                        Vector2 pos;

                        pos.x = dict[kScenePackageKeyPosition][kScenePackageKeyX].GetFloat() * pixelsPerPoint_;
                        pos.y = dict[kScenePackageKeyPosition][kScenePackageKeyY].GetFloat() * pixelsPerPoint_;

                        elem->position = vector2Builder.add(pos);
                    } else {
                        elem->position = defaultPositionIndex_;
                    }

                    if (dict.HasMember(kScenePackageKeyScale)) {
                        Vector2 scale;

                        scale.x = dict[kScenePackageKeyScale][kScenePackageKeyX].GetFloat();
                        scale.y = dict[kScenePackageKeyScale][kScenePackageKeyY].GetFloat();

                        elem->scale = vector2Builder.add(scale);
                    } else {
                        elem->scale = defaultScaleIndex_;
                    }

                    if (dict.HasMember(kScenePackageKeyRotation)) {
                        elem->rotation = dict[kScenePackageKeyRotation].GetFloat();
                    } else {
                        elem->rotation = 0;
                    }

                    if (dict.HasMember(kScenePackageKeySkew)) {
                        Vector2 skew;

                        skew.x = dict[kScenePackageKeySkew][kScenePackageKeyX].GetFloat();
                        skew.y = dict[kScenePackageKeySkew][kScenePackageKeyY].GetFloat();

                        elem->skew = vector2Builder.add(skew);
                    } else {
                        elem->skew = defaultSkewIndex_;
                    }

                    if (dict.HasMember(kScenePackageKeyCollRectScale)) {
                        Vector2 collisionRectScale;

                        collisionRectScale.x = dict[kScenePackageKeyCollRectScale][kScenePackageKeyX].GetFloat();
                        collisionRectScale.y = dict[kScenePackageKeyCollRectScale][kScenePackageKeyY].GetFloat();

                        elem->collisionRectScale = vector2Builder.add(collisionRectScale);
                    } else {
                        elem->collisionRectScale = defaultCollisionRectScaleIndex_;
                    }

                    if (dict.HasMember(kScenePackageKeyHidden)) {
                        elem->hidden = dict[kScenePackageKeyHidden].GetBool();
                    } else {
                        elem->hidden = false;
                    }

                    if (dict.HasMember(kScenePackageKeyFlags)) {
                        elem->flags = dict[kScenePackageKeyFlags].GetUint();
                    } else {
                        elem->flags = 0;
                    }
                }
            }

            strings_ = stringBuilder.createFixedArray();
            atlasSubtextures_ = texAtlasSubTexDefBuilder.createFixedArray();
            subTextureDefs_ = subTexDefBuilder.createFixedArray();
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
            keyframeIndices_ = keyframeIndicesBuilder.createFixedArray();
            
            keyframes_ = keyframeIntBuilder.createFixedArray();
            channels_ = channelRefIntBuilder.createFixedArray();

            computeMemoryUsage();

#if 0
            ScenePackageFormat format{*this};
            format.save("");
            if (!format_) {
                format_ = std::make_shared<ScenePackageFormat>();
            }
            format_->load(format.buffer_, format.bufferSize_);
            format.managed_ = true;

            NSURL *docUrl = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
            NSString *basename = [NSString stringWithFormat:@"%s.spkg", getName().c_str()];
            NSURL *url = [docUrl URLByAppendingPathComponent:basename];
            format_->save([url fileSystemRepresentation]);
            NSLog(@"HERE %@", [url absoluteString]);

            auto oldBaseDirectory = baseDirectory_;
            auto oldName = getName();
            auto handle = getHandle();
            destroy();
            initialize(handle, oldName);
            setBaseDirectory(oldBaseDirectory);
#if 1
            create(*format_, [this, callback](ScenePackage *) {
#if 1
                // TODO: This will be done later perhaps?
                loadAllTextures([this, callback]() {
                    if (callback) {
                        callback(this);
                    }
                });

#else
                if (callback) {
                    callback(this);
                }
#endif
            });
#endif
#else
#if TARGET_OS_IPHONE
            // TODO: This will be done later perhaps?
            loadAllTextures([this, callback]() {
                if (callback) {
                    callback(this);
                }
            });
#else
            if (callback) {
                callback(this);
            }
#endif /* TARGET_OS_IPHONE */
#endif
        } catch (std::exception& ex) {
#if DEBUG
            printf("Exception: could not unmarshal ScenePackage. %s\n", ex.what());
#endif
        }
    } else if (callback) {
        callback(this);
    }
}

void BGE::ScenePackage::create(const ScenePackageFormat& format, std::function<void(ScenePackage *)> callback) {
    if (status_ < ScenePackageStatus::Valid) {
        frameRate_ = format.frameRate_;
        pixelsPerPoint_ = format.pixelsPerPoint_;
        width_ = format.width_;
        height_ = format.height_;
        position_ = format.position_;

        emptyStringIndex_ = format.emptyStringIndex_;
        defaultPositionIndex_ = format.defaultPositionIndex_;
        defaultScaleIndex_ = format.defaultScaleIndex_;
        defaultSkewIndex_ = format.defaultSkewIndex_;
        defaultCollisionRectScaleIndex_ = format.defaultCollisionRectScaleIndex_;

        sourceIndex_ = format.sourceIndex_;

        // Strings
        strings_ = format.strings_;

        // Atlas Subtextures
        atlasSubtextures_ = format.atlasSubtextures_;

        // Sub Texture Defs
        subTextureDefs_ = format.subTextureDefs_;

        // Textures
        textures_ = format.textures_;

        // Text
        text_ = format.text_;

        // Animation Sequences
        animationSequences_ = format.animationSequences_;

        // Placements
        placements_ = format.placements_;

        // Masks
        masks_ = format.masks_;

        // Texture Masks
        textureMasks_ = format.textureMasks_;

        // Buttons
        buttons_ = format.buttons_;

        // Button States
        buttonStates_ = format.buttonStates_;

        // External Packages
        externalPackages_ = format.externalPackages_;

        // Auto Display Elements
        autoDisplayElements_ = format.autoDisplayElements_;

        // Rects
        rects_ = format.rects_;

        // Color Tranforms
        colorTransforms_ = format.colorTransforms_;

        // Color Matrices
        colorMatrices_ = format.colorMatrices_;

        // Vector2s
        vector2s_ = format.vector2s_;

        // Key Frame Indices
        keyframeIndices_ = format.keyframeIndices_;

        // Bounds
        bounds_ = format.bounds_;

        // Keyframes
        keyframes_ = format.keyframes_;

        // Channels
        channels_ = format.channels_;

        // Source
        source_ = strings_.addressOf(sourceIndex_);

        computeMemoryUsage();

        if (callback) {
            callback(this);
        }
    } else if (callback) {
        callback(this);
    }
}

void BGE::ScenePackage::saveAsSpkg(const std::string& filename) {
    ScenePackageFormat format{*this};
    format.save(filename);
}

void BGE::ScenePackage::loadAllTextures(std::function<void()> callback) {
    // Now get the file information
    loadTextures([this, callback]() {
        loadFonts([callback]() {
            if (callback) {
                callback();
            }
        });
    });
}

void BGE::ScenePackage::addSpaceHandleReference(SpaceHandle spaceHandle) {
    Game::getInstance()->getScenePackageService()->addSpaceHandleReference(spaceHandle, getHandle());
}

void BGE::ScenePackage::loadTextures(std::function<void()> callback) {
    if (!texturesLoaded_) {
        for (auto i=0;i<atlasSubtextures_.size();++i) {
            TextureAtlasSubTextureDefIntermediate *texAtlasSubTexDefInt = atlasSubtextures_.addressOf(i);
            auto name = strings_.addressOf(texAtlasSubTexDefInt->name);
            SubTextureDefIntermediate *subTexDefs = subTextureDefs_.addressOf(texAtlasSubTexDefInt->subTextDefStart);
            std::vector<SubTextureDef> atlasSubTexDefs;
            for (auto idx=0;idx<texAtlasSubTexDefInt->numSubTextures;++idx) {
                const SubTextureDefIntermediate& curr = subTexDefs[idx];
                auto subTexName = strings_.addressOf(curr.name);
                SubTextureDef subTexDef { subTexName, curr.x, curr.y, curr.width, curr.height, curr.rotated, false };
                atlasSubTexDefs.push_back(subTexDef);
            }
            subTextures_[name] = atlasSubTexDefs;
        }

        for (auto i=0;i<textures_.size();++i) {
            TextureReferenceIntermediate *texRefInt = textures_.addressOf(i);

            const char *name = strings_.addressOf(texRefInt->name);
            const char *filename = strings_.addressOf(texRefInt->filename);
            if (filename && strlen(filename)) {
                FilePath filePath(baseDirectory_);
                filePath.basename = filename;
                TextureQueueItem item{name, filePath, TextureFormat::RGBA8888, true};
                textureQueue_.push_back(item);
            }
        }

        if (textureQueue_.size() > 0) {
            for (auto const &tex : textureQueue_) {
                // Do we have subtextures affiliated with this?
                auto it = subTextures_.find(tex.name);

                if (it != subTextures_.end()) {
                    TextureAtlas *atlas;
                    std::shared_ptr<Error> error;
                    std::tie(atlas, error) = Game::getInstance()->getTextureService()->createTextureAtlasFromFile(getHandle(), tex.name, tex.filePath.filename(), it->second, tex.format, tex.createVbo);
                    if (atlas) {
                        loadedTextureAtlases_.push_back(atlas->getHandle());
                    }
                } else {
                    Texture *texture;
                    std::shared_ptr<Error> error;
                    std::tie(texture, error) = Game::getInstance()->getTextureService()->createTextureFromFile(getHandle(), tex.name, tex.filePath.filename(), tex.format);
                    if (texture) {
                        loadedTextures_.push_back(texture->getHandle());
                    }
                }
            }
            texturesLoaded_ = true;
            if (callback) {
                callback();
            }
        } else if (callback) {
            callback();
        }
    } else if (callback) {
        callback();
    }
}

void BGE::ScenePackage::loadFonts(std::function<void()> callback) {
    if (!fontsLoaded_) {
        for (auto i=0;i<text_.size();++i) {
            TextReferenceIntermediate *textRefInt = text_.addressOf(i);
            std::string fontName;
            auto size = textRefInt->size;
            bool found = false;

            fontName = strings_.addressOf(textRefInt->fontName);

            for (auto const &f : fontQueue_) {
                if (f.first == fontName && static_cast<int32_t>(f.second) == size) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                fontQueue_.push_back(std::make_pair(fontName, size));
            }
        }
        if (fontQueue_.size() > 0) {
            for (auto const &font : fontQueue_) {
                FontHandle fontHandle;
                std::shared_ptr<Error> error;
                std::tie(fontHandle, error) = Game::getInstance()->getFontService()->createFont(font.first, font.second, handle_);
                if (!fontHandle.isNull()) {
                    loadedFonts_.push_back(fontHandle);
                }
            }
            fontsLoaded_ = true;
            if (callback) {
                callback();
            }
        } else if (callback) {
            callback();
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

BGE::TextureMaskReference *BGE::ScenePackage::getTextureMaskReference(std::string name) {
    const char *cstr = name.c_str();
    
    // TODO: We will sort the names, then binary search the names here later
    const char **names = textureMaskNames_.baseAddress();
    
    for (auto i=0;i<textureMaskNames_.size();i++) {
        if (!strcmp(names[i], cstr)) {
            return textureMaskRefs_.addressOf(*textureMaskIndices_.addressOf(i));
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

bool BGE::ScenePackage::hasSubTexture(std::string name) {
    bool has = false;
    auto it = subTextures_.find(name);
    
    if (it != subTextures_.end()) {
        has = true;
    }
    
    return has;
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

std::string BGE::ScenePackage::textureFilename(const std::string& name) {
    for (auto& t : textureQueue_) {
        if (name == t.name) {
            return t.filePath.filename();
        }
    }
    return std::string();
}

void BGE::ScenePackage::saveAsBinary(const std::string& filename) {
    if (!format_) {
        // Create a format_ so we can export a binary
        format_ = std::make_shared<ScenePackageFormat>(*this);
        format_->save(filename);
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
