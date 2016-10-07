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
#include "BoundingBoxComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "MaskComponent.h"
#include "PlacementComponent.h"

uint32_t BGE::AnimationChannelComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::AnimationChannelComponent::typeId_ = Component::InvalidTypeId;
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
        auto package = Game::getInstance()->getScenePackageService()->getScenePackage(this->channel->scenePackage);
        
        // Now setup the proper render component
        
#ifdef NOT_YET
        // TODO: Do we do this later?
        NSLog(@"Channel reference %s/%d", this->channel->reference, this->channel->referenceType);
#endif
        switch (this->channel->referenceType) {
            case GfxReferenceTypeSprite: {
                auto sprite = space->createComponent<SpriteRenderComponent>();
                auto texRef = package->getTextureReference(this->channel->reference);
                auto bbox = space->createComponent<BoundingBoxComponent>();
                
                // TODO: Remove any older render components
                gameObj->addComponent(sprite);
                gameObj->addComponent(bbox);
                
                sprite->setTextureReference(texRef);
            }
                break;
                
            case GfxReferenceTypeButton:{
                auto button = space->createComponent<ButtonComponent>();
                auto bbox = space->createComponent<BoundingBoxComponent>();
                
                auto buttonRef = package->getButtonReference(this->channel->reference);
                gameObj->addComponent(button);
                gameObj->addComponent(bbox);
                
                button->setButtonReference(buttonRef);
            }
                break;
                
            case GfxReferenceTypeAnimationSequence: {
#ifdef NOT_YET
                NSLog(@"WHY");
#endif
            }
                break;
                
            case GfxReferenceTypeMask: {
                auto mask = space->createComponent<MaskComponent>();
                auto maskRef = package->getMaskReference(this->channel->reference);
                
                gameObj->addComponent(mask);
                mask->setMaskReference(maskRef);
            }
                break;
                
            case GfxReferenceTypeKeyframe: {
                // Keyframe needs to be filled out later
                //auto seq =
#ifdef NOT_YET
                NSLog(@"HERE");
#endif
            }
                break;
                
            case GfxReferenceTypeText: {
                TextReference *textRef = package->getTextReference(this->channel->reference);
                
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
                
            case GfxReferenceTypePlacement: {
                PlacementReference *placementRef = package->getPlacementReference(this->channel->reference);
                
                if (placementRef) {
                    auto placement = space->createComponent<PlacementComponent>();
                    
                    gameObj->addComponent(placement);
                    
                    placement->setPlacementReference(placementRef);
                }
            }
                break;
                
            default:
                assert(false);
                break;
        }
    } else {
#ifdef NOT_YET
        if (this->channel) {
            NSLog(@"You fail %lx %lx %lx", (unsigned long) gameObj, (unsigned long) this->channel, (unsigned long) this->channel->referenceData);
        } else {
            NSLog(@"You fail %lx", (unsigned long)  gameObj);
        }
#endif
    }
}
