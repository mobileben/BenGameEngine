//
//  AnimationChannelComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AnimationChannelComponent.h"
#include "Game.h"
#include "GameObject.h"
#include "SpriteRenderComponent.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "MaskComponent.h"

uint32_t BGE::AnimationChannelComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::AnimationChannelComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::AnimationChannelComponent::type_index_ = typeid(BGE::AnimationChannelComponent);

void BGE::AnimationChannelComponent::setGameObjectHandle(GameObjectHandle handle) {
    Component::setGameObjectHandle(handle);
    
    updateReference();
}

void BGE::AnimationChannelComponent::setAnimationChannelReference(const AnimationChannelReference *animChanRef) {
    this->channel = animChanRef;
    
    updateReference();
}

void BGE::AnimationChannelComponent::updateReference() {
    auto space = getSpace();
    auto gameObj = space->getGameObject(getGameObjectHandle());
    
    if (this->channel) {
        // Now setup the proper render component
        
        // TODO: Do we do this later?
        NSLog(@"Channel reference %s/%d", this->channel->reference, this->channel->referenceType);
        switch (this->channel->referenceType) {
            case GfxReferenceTypeSprite: {
                auto sprite = space->createComponent<SpriteRenderComponent>();
                auto texRef = Game::getInstance()->getScenePackageService()->getTextureReference(this->channel->reference);
                auto bbox = space->createComponent<BoundingBoxComponent>();
                
                // TODO: Remove any older render components
                gameObj->addComponent(sprite);
                gameObj->addComponent(bbox);
                
                sprite->setTextureReference(texRef);
            }
                break;
                
            case GfxReferenceTypeButton:{
                auto button = space->createComponent<ButtonComponent>();
                auto input = space->createComponent<InputTouchComponent>();
                auto bbox = space->createComponent<BoundingBoxComponent>();
                
                auto buttonRef = Game::getInstance()->getScenePackageService()->getButtonReference(this->channel->reference);
                gameObj->addComponent(button);
                gameObj->addComponent(input);
                gameObj->addComponent(bbox);
                
                button->setButtonReference(buttonRef);
            }
                break;
                
            case GfxReferenceTypeAnimationSequence: {
                NSLog(@"WHY");
            }
                break;
                
            case GfxReferenceTypeMask: {
                auto mask = space->createComponent<MaskComponent>();
                auto maskRef = Game::getInstance()->getScenePackageService()->getMaskReference(this->channel->reference);
                
                gameObj->addComponent(mask);
                mask->setMaskReference(maskRef);
            }
                break;
                
            case GfxReferenceTypeKeyframe: {
                // Keyframe needs to be filled out later
                //auto seq =
                NSLog(@"HERE");
            }
                break;
                
            case GfxReferenceTypeText: {
                TextReference *textRef = Game::getInstance()->getScenePackageService()->getTextReference(this->channel->reference);
                
                if (textRef && !textRef->fontHandle.isNull()) {
                    auto text = space->createComponent<TextComponent>();
                    auto bbox = space->createComponent<BoundingBoxComponent>();

                    gameObj->addComponent(text);
                    gameObj->addComponent(bbox);
                    
                    text->setTextReference(*textRef);
                }
                // TODO: Remove any older render components
            }
                break;
                
            case GfxReferenceTypePlacement:
                // No-op
                break;
                
            default:
                assert(false);
                break;
        }
    } else {
        if (this->channel) {
            NSLog(@"You fail %lx %lx %lx", (unsigned long) gameObj, (unsigned long) this->channel, (unsigned long) this->channel->referenceData);
        } else {
            NSLog(@"You fail %lx", (unsigned long)  gameObj);
        }
    }
}
