//
//  AnimatorComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimatorComponent.h"
#include "GameObject.h"
#include "AnimationSequenceComponent.h"
#include "TransformComponent.h"
#include "AnimationChannelComponent.h"
#include "ChannelFrameAnimatorComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "BoundingBoxComponent.h"
#include "Material.h"
#include "LineRenderComponent.h"
#include "PolyLineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "FrameAnimatorComponent.h"

uint32_t BGE::AnimatorComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::AnimatorComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimatorComponent::type_index_ = typeid(BGE::AnimatorComponent);

BGE::AnimatorComponent::AnimatorComponent() : Component(), state(AnimState::Done), currentFrame(0), iterations(1), frameRemainderTime(0), secPerFrame(1.0/30.0), speed(1), forward(true) {
}

void BGE::AnimatorComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    state = AnimState::Done;
    startFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    iterations = 0;
    frameRemainderTime = 0.0F;
    speed = 1.0F;
    forward = true;
}

void BGE::AnimatorComponent::reset() {
    auto space = getSpace();

    setFrame(space, 0);
    iterations = 1;
    state = AnimState::Done;

    auto gameObj = getGameObject(space);
    auto seq = gameObj->getComponent<AnimationSequenceComponent>(space);
    
    if (seq->frameRate != 0) {
        secPerFrame = 1.0 / (float) seq->frameRate;
    } else {
        secPerFrame = 0;
    }
    
    frameRemainderTime = secPerFrame;
}

void BGE::AnimatorComponent::setFrame(int32_t frame) {
    auto space = getSpace();
    setFrame(space, frame);
}

void BGE::AnimatorComponent::setFrame(Space *space, int32_t frame) {
    auto gameObjHandle = getGameObjectHandle();
    auto gameObj = space->getGameObject(gameObjHandle);

    // We need to update frames
    currentFrame = frame;

    if (gameObj) {
        auto seq = gameObj->getComponent<AnimationSequenceComponent>(space);
        size_t numChannels = seq->channels.size();
        
        for (size_t i=0;i<numChannels;i++) {
            gameObj = space->getGameObject(gameObjHandle);
            seq = gameObj->getComponent<AnimationSequenceComponent>(space);
            
            auto childHandle = seq->channels[i];
            auto childObj = space->getGameObject(childHandle);
            
            animateChannel(space, childObj, frame);
        }
        
        gameObj = space->getGameObject(gameObjHandle);
        seq = gameObj->getComponent<AnimationSequenceComponent>(space);
        
        // Update the bounds
        for (uint32_t i=0;i<seq->numBounds;i++) {
            auto bounds = seq->bounds[i];
            
            if (bounds.startFrame >= i && i < (bounds.startFrame + bounds.totalFrames)) {
                auto bbox = gameObj->getComponent<BoundingBoxComponent>(space);
                
                if (bbox) {
                    bbox->x = bounds.bounds->x;
                    bbox->y = bounds.bounds->y;
                    bbox->width = bounds.bounds->w;
                    bbox->height = bounds.bounds->h;
                }
                break;
            }
        }
    }
}

void BGE::AnimatorComponent::setToLastFrame() {
    auto space = getSpace();
    auto gameObj = getGameObject(space);

    if (gameObj) {
        auto seq = gameObj->getComponent<AnimationSequenceComponent>(space);
        setFrame(space, seq->totalFrames - 1);
    }
}

void BGE::AnimatorComponent::animateChannel(Space *space, GameObject *gameObj, int32_t frame) {
    auto xform = gameObj->getComponent<TransformComponent>(space);
    auto channel = gameObj->getComponent<AnimationChannelComponent>(space);
    auto animator = gameObj->getComponent<ChannelFrameAnimatorComponent>(space);
    int32_t origFrame = animator->currKeyframe;
    int32_t channelFrame = origFrame;
    AnimationKeyframeReference *keyframe = channel->channel->keyframes[channelFrame];
    bool hide = false;
    
    if (frame < static_cast<int32_t>(keyframe->startFrame)) {
        // We need to start stepping back keyframes
        while (--channelFrame >= 0) {
            keyframe = channel->channel->keyframes[channelFrame];
            
            if (frame >= static_cast<int32_t>(keyframe->startFrame)) {
                break;
            }
        }
        
        if (channelFrame < 0) {
            channelFrame = 0;
            keyframe = channel->channel->keyframes[channelFrame];
            hide = true;
        }
    } else if (frame > static_cast<int32_t>(keyframe->startFrame + keyframe->totalFrames) - 1) {
        // We need to start stepping forward keyframes
        while (++channelFrame < static_cast<int32_t>(channel->channel->numKeyframes)) {
            keyframe = channel->channel->keyframes[channelFrame];
            
            if (frame < static_cast<int32_t>(keyframe->startFrame + keyframe->totalFrames)) {
                break;
            }
        }
        
        if (channelFrame >= static_cast<int32_t>(channel->channel->numKeyframes)) {
            channelFrame = static_cast<int32_t>(channel->channel->numKeyframes) - 1;
            keyframe = channel->channel->keyframes[channelFrame];
            hide = true;
        }
    } else {
        // We have the correct keyframe
    }
    
    if (hide) {
        xform->setVisibility(false);
    } else {
        xform->setVisibility(true);
        
        Material *material = nullptr;

        if (gameObj->getComponent<LineRenderComponent>(space)) {
            auto render = gameObj->getComponent<LineRenderComponent>(space);
            material = render->getMaterial();
        } else if (gameObj->getComponent<FlatRectRenderComponent>(space)) {
            auto render = gameObj->getComponent<FlatRectRenderComponent>(space);
            material = render->getMaterial();
        } else if (gameObj->getComponent<SpriteRenderComponent>(space)) {
            auto render = gameObj->getComponent<SpriteRenderComponent>(space);
            material = render->getMaterial();
        } else if (gameObj->getComponent<TextComponent>(space)) {
            auto render = gameObj->getComponent<TextComponent>(space);
            material = render->getMaterial();
        } else if (gameObj->getComponent<PolyLineRenderComponent>(space)) {
            auto render = gameObj->getComponent<PolyLineRenderComponent>(space);
            material = render->getMaterial();
        }

        animator->currKeyframe = channelFrame;

        // Update our transform
        if (keyframe->position) {
            xform->setPosition(*keyframe->position);
        } else {
            xform->setX(0);
            xform->setY(0);
        }

        xform->setZ(keyframe->order);

        if (keyframe->scale) {
            xform->setScale(*keyframe->scale);
        } else {
            xform->setScaleX(1);
            xform->setScaleY(1);
        }
        if (keyframe->skew) {
            xform->setSkew(*keyframe->skew);
        } else {
            xform->setSkew(Vector2{{0, 0}});
        }
        if (keyframe->collisionRectScale) {
            xform->setCollisionRectScale(*keyframe->collisionRectScale);
        } else {
            xform->setCollisionRectScale(Vector2{{1,1}});
        }
        auto colorMatrix = gameObj->getComponent<ColorMatrixComponent>(space);
        auto colorTransform = gameObj->getComponent<ColorTransformComponent>(space);

        if (keyframe->colorMatrix) {
            if (colorMatrix) {
                colorMatrix->matrix = *keyframe->colorMatrix;
            } else {
                colorMatrix = space->createComponent<ColorMatrixComponent>();
                gameObj->addComponent(colorMatrix);
            }

            colorMatrix->matrix = *keyframe->colorMatrix;

            if (material) {
                material->setColorMatrix(*keyframe->colorMatrix);
            }
        } else {
            if (colorMatrix) {
                gameObj->removeComponent<ColorMatrixComponent>();
            }
        }

        if (keyframe->colorTransform) {
            if (colorTransform) {
                colorTransform->transform = *keyframe->colorTransform;
            } else {
                colorTransform = space->createComponent<ColorTransformComponent>();
                gameObj->addComponent(colorTransform);
            }

            colorTransform->transform = *keyframe->colorTransform;
        } else {
            if (colorTransform) {
                gameObj->removeComponent<ColorTransformComponent>();
            }
        }

        xform->setRotationInDegrees(keyframe->rotation);
        xform->forceDirty();

        // If this is a Keyframe reference, update
        if (channel->channel->referenceType == GfxReferenceTypeKeyframe) {
            // Find the appropriate child
            uint32_t numChildren = xform->getNumChildren();
            for (uint32_t i=0;i<numChildren;i++) {
                auto childXform = xform->childAtIndex(i);
                if (childXform->hasGameObject()) {
                    auto childObj = childXform->getGameObject(space);

                    childXform->forceDirty();
                    // TODO: Have some better means of identifying the right child. For now brute force it
                    if (childObj) {
                        auto childAnimator = childObj->getComponent<FrameAnimatorComponent>(space);

                        if (childAnimator) {
                            auto childSeq = childObj->getComponent<AnimationSequenceComponent>(space);
                            animateSequenceByFrame(space, childSeq, childAnimator, keyframe->frame);

                            // Update the bounds
                            for (uint32_t bi=0;i<childSeq->numBounds;bi++) {
                                auto bounds = childSeq->bounds[bi];

                                if (bounds.startFrame >= bi && bi < (bounds.startFrame + bounds.totalFrames)) {
                                    auto bbox = childObj->getComponent<BoundingBoxComponent>(space);

                                    if (!bbox) {
                                        bbox = space->createComponent<BGE::BoundingBoxComponent>();
                                        childObj->addComponent(bbox);
                                    }

                                    bbox->x = bounds.bounds->x;
                                    bbox->y = bounds.bounds->y;
                                    bbox->width = bounds.bounds->w;
                                    bbox->height = bounds.bounds->h;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void BGE::AnimatorComponent::animateSequenceByFrame(Space *space, AnimationSequenceComponent *seq, FrameAnimatorComponent *animator, int32_t frame) {
    animator->currentFrame = frame;

    for (auto const &childHandle : seq->channels) {
        auto child = space->getGameObject(childHandle);
        animateChannel(space, child, frame);
    }
}

void BGE::AnimatorComponent::play(int32_t iterations, bool forward, float speed) {
    auto space = getSpace();
    auto gameObj = getGameObject(space);
    auto seq = gameObj->getComponent<AnimationSequenceComponent>(space);

    if (seq->frameRate != 0) {
        secPerFrame = 1.0 / (float) seq->frameRate;
    } else {
        secPerFrame = 1.0 / 30.0;   // Default to 30 fps
    }

    this->state = AnimState::Playing;
    this->iterations = iterations;
    this->forward = forward;
    this->speed = speed;
    this->startFrame = 0;
    this->endFrame = seq->totalFrames - 1;
    this->frameRemainderTime = 0;

    if (forward) {
        this->setFrame(space, 0);
    } else {
        this->setFrame(space, seq->totalFrames - 1);
    }
}

void BGE::AnimatorComponent::playToFrame(int32_t endFrame, float speed) {
    auto space = getSpace();
    auto gameObj = getGameObject(space);
    auto seq = gameObj->getComponent<AnimationSequenceComponent>(space);

    if (seq->frameRate != 0) {
        secPerFrame = 1.0 / (float) seq->frameRate;
    } else {
        secPerFrame = 1.0 / 30.0;   // Default to 30 fps
    }

    if (endFrame >= static_cast<int32_t>(seq->totalFrames)) {
        endFrame = seq->totalFrames - 1;
    }
    
    this->state = AnimState::Playing;
    this->iterations = 1;
    this->forward = true;
    this->speed = speed;
    this->startFrame = 0;
    this->endFrame = endFrame;
    this->frameRemainderTime = 0;

    this->setFrame(space, 0);
}

void BGE::AnimatorComponent::reverse(float speed) {
    this->state = AnimState::Playing;
    this->iterations = 1;
    this->forward = !this->forward;
    this->speed = speed;
}

void BGE::AnimatorComponent::pause() {
    this->state = AnimState::Paused;
}

void BGE::AnimatorComponent::resume(float speed) {
    if (this->state == AnimState::Paused) {
        this->state = AnimState::Playing;
        this->speed = speed;
    }
}

void BGE::AnimatorComponent::stop() {
    // TODO: Do we send this event to listeners?
    this->state = AnimState::Done;
}
