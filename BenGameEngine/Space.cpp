//
//  Space.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Space.h"
#include "Game.h"
#include "GameObjectService.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"

BGE::Space::Space() : NamedObject(), visible_(false), order_(0), updatable_(true) {
}

BGE::Space::Space(ObjectId spaceId) : NamedObject(spaceId), visible_(false), order_(0), updatable_(true) {
}

BGE::Space::Space(ObjectId spaceId, std::string name) : NamedObject(spaceId, name), visible_(false), order_(0), updatable_(true) {
}

void BGE::Space::initialize(SpaceHandle handle, std::string name) {
    spaceHandle_ = handle;
    setName(name);

    gameObjectService_ = std::make_shared<GameObjectService>();
    gameObjectService_->setSpaceHandle(spaceHandle_);
    
    componentService_ = std::make_shared<ComponentService>();
    componentService_->setSpaceHandle(spaceHandle_);
}

BGE::GameObject *BGE::Space::createGameObject(std::string name ) {
    return gameObjectService_->createGameObject(name);
}

BGE::GameObjectHandle BGE::Space::getGameObjectHandle(ObjectId objId) {
    return gameObjectService_->getGameObjectHandle(objId);
}

BGE::GameObjectHandle BGE::Space::getGameObjectHandle(std::string name) {
    return gameObjectService_->getGameObjectHandle(name);
}

BGE::GameObject *BGE::Space::getGameObject(ObjectId objId) {
    return gameObjectService_->getGameObject(objId);
}

BGE::GameObject *BGE::Space::getGameObject(std::string name) {
    return gameObjectService_->getGameObject(name);
}

BGE::GameObject *BGE::Space::getGameObject(GameObjectHandle handle) {
    return gameObjectService_->getGameObject(handle);
}

void BGE::Space::removeGameObject(GameObjectHandle handle) {
    gameObjectService_->removeGameObject(handle);
}

const std::vector<BGE::GameObjectHandle>& BGE::Space::getGameObjects() const {
    return gameObjectService_->getGameObjects();
}

BGE::GameObject *BGE::Space::createAnimSequence(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto boundingBox = createComponent<BoundingBoxComponent>();
        auto animSeq = createComponent<AnimationSequenceComponent>();
        auto animator = createComponent<AnimatorComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(boundingBox);
        obj->addComponent(animSeq);
        obj->addComponent(animator);

        animSeq->setAnimationSequenceReference(animSeqRef);
        animator->setFrame(0, true);
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createAnimChannel(std::string name, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate delegate) {
    if (channelRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto channel = createComponent<AnimationChannelComponent>();
        auto animator = createComponent<ChannelFrameAnimatorComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(channel);
        obj->addComponent(animator);
        
        channel->setAnimationChannelReference(channelRef);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createFrameAnimSequence(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto animSeq = createComponent<AnimationSequenceComponent>();
        auto animator = createComponent<FrameAnimatorComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(animSeq);
        obj->addComponent(animator);
        
        animSeq->setAnimationSequenceReference(animSeqRef);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createButton(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    ButtonReference *buttonRef;
    
    if (package) {
        buttonRef = package->getButtonReference(name);
    } else {
        buttonRef = Game::getInstance()->getScenePackageService()->getButtonReference(name);
    }
    
    if (buttonRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto button = createComponent<ButtonComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        auto input = createComponent<InputTouchComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(button);
        obj->addComponent(input);
        obj->addComponent(bbox);
        
        button->setButtonReference(buttonRef);

        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createExternalReference(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    ExternalPackageReference *extRef;
    
    if (package) {
        extRef = package->getExternalReference(name);
    } else {
        extRef = Game::getInstance()->getScenePackageService()->getExternalReference(name);
    }
    
    if (extRef) {
        auto extPackage = Game::getInstance()->getScenePackageService()->getScenePackage(extRef->externalPackage);

        if (extPackage) {
            std::string refName = extRef->name;
            auto type = extPackage->getReferenceType(refName);

            printf("XXXXX %s/%d", refName.c_str(), type);
            
            switch (type) {
                case GfxReferenceTypeAnimationSequence:
                    return createAnimSequence(refName);
                    
                case GfxReferenceTypeButton:
                    return createButton(refName);
                    
                case GfxReferenceTypeExternalReference:
                    return createExternalReference(refName);
                    
                case GfxReferenceTypeMask:
                    return createMask(refName);
                    
                case GfxReferenceTypePlacement:
                    return createPlacement(refName);
                    
                case GfxReferenceTypeSprite:
                    return createSprite(refName);
                    
                case GfxReferenceTypeText:
                    return createText(refName);
                    
                case GfxReferenceTypeTextureMask:
                default:
                    assert(false);
                break;
            }
        }
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createMask(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    MaskReference *maskRef;
    
    if (package) {
        maskRef = package->getMaskReference(name);
    } else {
        maskRef = Game::getInstance()->getScenePackageService()->getMaskReference(name);
    }
    
    if (maskRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto mask = createComponent<MaskComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(mask);

        mask->setMaskReference(maskRef);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createSprite(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextureReference *texRef;
    
    if (package) {
        texRef = package->getTextureReference(name);
    } else {
        texRef = Game::getInstance()->getScenePackageService()->getTextureReference(name);
    }
    
    if (texRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto sprite = createComponent<SpriteRenderComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(sprite);
        obj->addComponent(bbox);
        
        sprite->setTextureReference(texRef);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createText(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextReference *textRef;
    
    if (package) {
        textRef = package->getTextReference(name);
    } else {
        textRef = Game::getInstance()->getScenePackageService()->getTextReference(name);
    }
    
    if (textRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        auto text = createComponent<TextComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(text);
        obj->addComponent(bbox);
        
        text->setTextReference(textRef);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createPlacement(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    PlacementReference *placementRef;
    
    if (package) {
        placementRef = package->getPlacementReference(name);
    } else {
        placementRef = Game::getInstance()->getScenePackageService()->getPlacementReference(name);
    }
    
    if (placementRef) {
        auto obj = createGameObject(name);
        auto xform = createComponent<TransformComponent>();
        
        obj->addComponent(xform);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

void BGE::Space::createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
    auto root = getGameObject(rootHandle);
    
    if (package) {
        auto autoDisplayList = package->getAutoDisplayList();
        auto num = package->getAutoDisplayListSize();
        std::map<uint32_t, std::vector<GameObjectHandle>> createdObjectHandles;
        std::vector<GameObjectHandle> rootObjHandles;
        
        for (auto i=0;i<GfxReferenceTypeLast;i++) {
            createdObjectHandles[i] = std::vector<GameObjectHandle>();
        }
        
        for (auto i=0;i<num;i++) {
            auto elem = &autoDisplayList[i];
            GameObject *obj;
            
            switch (elem->referenceType) {
                case GfxReferenceTypeAnimationSequence:
                    obj = createAnimSequence(elem->reference);
                    break;
                    
                case GfxReferenceTypeButton:
                    obj = createButton(elem->reference);
                    break;
                
                case GfxReferenceTypeExternalReference:
                    obj = createExternalReference(elem->reference);
                    break;
                    
                case GfxReferenceTypeMask:
                    obj = createMask(elem->reference);
                    break;
                    
                case GfxReferenceTypePlacement:
                    obj = createPlacement(elem->reference);
                    break;
                    
                case GfxReferenceTypeSprite:
                    obj = createSprite(elem->reference);
                    break;
                    
                case GfxReferenceTypeText:
                    obj = createText(elem->reference);
                    break;
                    
                case GfxReferenceTypeTextureMask:
                    break;
                    
                default:
                    assert(false);
                    break;
            }
            
            if (obj) {
                auto xform = obj->getComponent<TransformComponent>();
                
                if (elem->position) {
                    xform->setPosition(*elem->position);
                    printf("XXXX %s %f %f\n", obj->getName().c_str(), elem->position->x, elem->position->y);
                }
                
                if (elem->scale) {
                    xform->setScale(*elem->scale);
                }
                
                xform->setRotation(elem->rotation);
                
                if (elem->colorMatrix) {
                    
                }
                
                if (elem->colorTransform) {
                    
                }
                
                if (elem->hidden) {
                    xform->setVisibility(false);
                }
            }

            createdObjectHandles[elem->referenceType].push_back(obj->getHandle());
            rootObjHandles.push_back(obj->getHandle());
        }
        
        for (auto i=0;i<GfxReferenceTypeLast;i++) {
            auto itemHandles = createdObjectHandles[i];
            
            if (itemHandles.size() > 0) {
                std::function<void(GameObject *)> callback = nullptr;
                
                switch (i) {
                    case GfxReferenceTypeAnimationSequence:
                        callback = delegate.animationSequenceCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeButton:
                        callback = delegate.buttonCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeExternalReference:
                        break;
                        
                    case GfxReferenceTypeMask:
                        callback = delegate.maskCreatedHandler;
                        break;
                        
                    case GfxReferenceTypePlacement:
                        callback = delegate.placementCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeSprite:
                        callback = delegate.spriteCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeText:
                        callback = delegate.textCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeTextureMask:
                        callback = delegate.textureMaskCreatedHandler;
                        break;
                        
                    default:
                        assert(false);
                        break;
                }
                
                if (callback) {
                    for (auto itemHandle : itemHandles) {
                        auto item = getGameObject(itemHandle);
                        callback(item);
                    }
                }
            }
        }
        
        TransformComponent *rootXform = nullptr;
        
        if (root) {
            rootXform = root->getComponent<TransformComponent>();
        }
        
        for (auto objHandle : rootObjHandles) {
            auto obj = getGameObject(objHandle);
            
            if (rootXform) {
                auto xform = obj->getComponent<TransformComponent>();
                rootXform->addChild(xform);
            }
            
            obj->setActive(true);
        }

        if (delegate.completionHandler) {
            delegate.completionHandler(root);
        }
    } else {
        if (delegate.completionHandler) {
            delegate.completionHandler(root);
        }
    }
}
