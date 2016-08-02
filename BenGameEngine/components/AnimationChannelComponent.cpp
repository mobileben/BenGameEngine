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

uint32_t BGE::AnimationChannelComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::AnimationChannelComponent::type_index_ = typeid(BGE::AnimationChannelComponent);

std::shared_ptr<BGE::AnimationChannelComponent> BGE::AnimationChannelComponent::create(ObjectId componentId) {
    return std::make_shared<AnimationChannelComponent>(private_key{}, componentId);
}

BGE::AnimationChannelComponent::AnimationChannelComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), channel(nullptr) {
}

void BGE::AnimationChannelComponent::setGameObject(std::shared_ptr<GameObject> gameObject) {
    Component::setGameObject(gameObject);
    
    updateReference();
}

void BGE::AnimationChannelComponent::setAnimationChannelReference(const AnimationChannelReference *animChanRef) {
    this->channel = animChanRef;
    
    updateReference();
}

void BGE::AnimationChannelComponent::updateReference() {
    std::shared_ptr<GameObject> gameObj = getGameObject().lock();
    
    if (gameObj && this->channel) {
        // Now setup the proper render component
        auto space = getSpace();
        
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
                
            default:
                assert(false);
                break;
        }
    } else {
        if (this->channel) {
            NSLog(@"You fail %x %x %x", gameObj.get(), this->channel, this->channel->referenceData);
        } else {
            NSLog(@"You fail %x", gameObj.get());
        }
    }
}
