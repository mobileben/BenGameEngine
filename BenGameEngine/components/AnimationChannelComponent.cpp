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

std::shared_ptr<BGE::AnimationChannelComponent> BGE::AnimationChannelComponent::create(uint64_t componentId) {
    return std::make_shared<AnimationChannelComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::AnimationChannelComponent> BGE::AnimationChannelComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<AnimationChannelComponent>(private_key{}, componentId, name);
}

BGE::AnimationChannelComponent::AnimationChannelComponent(struct private_key const& key, uint64_t componentId) : Component(componentId) {
}

BGE::AnimationChannelComponent::AnimationChannelComponent(struct private_key const& key, uint64_t componentId, std::string name) : Component(componentId, name) {
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
        
        auto space = getSpace().lock();
        
        // TODO: Do we do this later?
        switch (this->channel->referenceType) {
            case GfxReferenceTypeSprite: {
                auto sprite = space->createComponent<SpriteRenderComponent>(this->channel->reference);
                auto texRef = Game::getInstance()->getScenePackageService()->getTextureReference(this->channel->reference);
                
                sprite->setTextureRef(texRef);
                // TODO: Remove any older render components
                gameObj->addComponent(sprite);
                NSLog(@"XXX %s/%ld", sprite->getName().c_str(), gameObj->getInstanceId());
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
                }

                auto text = Game::getInstance()->getScenePackageService()->createTextComponent(this->channel->reference);
                // TODO: Remove any older render components
                gameObj->addComponent(text);
            }
                break;
                
            default:
                break;
        }
    }
}
