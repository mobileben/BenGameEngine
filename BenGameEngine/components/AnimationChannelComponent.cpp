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

uint32_t BGE::AnimationChannelComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::AnimationChannelComponent::type_index_ = typeid(BGE::AnimationChannelComponent);

std::shared_ptr<BGE::AnimationChannelComponent> BGE::AnimationChannelComponent::create(ObjectId componentId) {
    return std::make_shared<AnimationChannelComponent>(private_key{}, componentId);
}

BGE::AnimationChannelComponent::AnimationChannelComponent(struct private_key const& key, ObjectId componentId) : Component(componentId) {
}

void BGE::AnimationChannelComponent::setGameObject(std::shared_ptr<GameObject> gameObj) {
    Component::setGameObject(gameObj);
    
    updateReference();
}

void BGE::AnimationChannelComponent::setAnimationChannelReference(const AnimationChannelReference *animChanRef) {
    this->channel = animChanRef;
    
    updateReference();
}

void BGE::AnimationChannelComponent::updateReference() {
    std::shared_ptr<GameObject> gameObj = getGameObject().lock();
    
    if (gameObj && this->channel && this->channel->referenceData) {
        // Now setup the proper render component
        
        if (gameObj->getName() == "instance2") {
            NSLog(@"HERE");
        }
        
        auto space = getSpace();
        
        // TODO: Do we do this later?
        switch (this->channel->referenceType) {
            case GfxReferenceTypeSprite: {
                auto sprite = space->createComponent<SpriteRenderComponent>();
                auto texRef = Game::getInstance()->getScenePackageService()->getTextureReference(this->channel->reference);
                
                sprite->setTextureRef(texRef);
                // TODO: Remove any older render components
                gameObj->addComponent(sprite);
            }
                break;
                
            case GfxReferenceTypeAnimationSequence: {
                
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
                    
                    text->setTextReference(*textRef);
                    gameObj->addComponent(text);
                }
                // TODO: Remove any older render components
            }
                break;
                
            default:
                break;
        }
    }
}
