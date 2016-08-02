//
//  Space.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Space.h"
#include "Game.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"

BGE::Space::Space( ObjectId spaceId) : NamedObject(spaceId), visible_(false), order_(0), updatable_(true) {
}

BGE::Space::Space(ObjectId spaceId, std::string name) : NamedObject(spaceId, name), visible_(false), order_(0), updatable_(true) {
}

void BGE::Space::initialize(SpaceHandle handle, ObjectId spaceId, std::string name) {
    spaceHandle_ = handle;
    setInstanceId(spaceId);
    setName(name);

    gameObjectService_ = std::make_shared<GameObjectService>();
    gameObjectService_->setSpaceHandle(spaceHandle_);
    
    componentService_ = std::make_shared<ComponentService>();
    componentService_->setSpaceHandle(spaceHandle_);
}


std::shared_ptr<BGE::GameObject> BGE::Space::createAnimSequence(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createAnimChannel(std::string name, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate delegate) {
    if (channelRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createFrameAnimSequence(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createButton(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    ButtonReference *buttonRef;
    
    if (package) {
        buttonRef = package->getButtonReference(name);
    } else {
        buttonRef = Game::getInstance()->getScenePackageService()->getButtonReference(name);
    }
    
    if (buttonRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createMask(std::string name, ScenePackageHandle handle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    MaskReference *maskRef;
    
    if (package) {
        maskRef = package->getMaskReference(name);
    } else {
        maskRef = Game::getInstance()->getScenePackageService()->getMaskReference(name);
    }
    
    if (maskRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createSprite(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextureReference *texRef;
    
    if (package) {
        texRef = package->getTextureReference(name);
    } else {
        texRef = Game::getInstance()->getScenePackageService()->getTextureReference(name);
    }
    
    if (texRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createText(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextReference *textRef;
    
    if (package) {
        textRef = package->getTextReference(name);
    } else {
        textRef = Game::getInstance()->getScenePackageService()->getTextReference(name);
    }
    
    if (textRef) {
        auto obj = createObject<GameObject>(name);
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

std::shared_ptr<BGE::GameObject> BGE::Space::createPlacement(std::string name, ScenePackageHandle handle) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    PlacementReference *placementRef;
    
    if (package) {
        placementRef = package->getPlacementReference(name);
    } else {
        placementRef = Game::getInstance()->getScenePackageService()->getPlacementReference(name);
    }
    
    if (placementRef) {
        auto obj = createObject<GameObject>(name);
        auto xform = createComponent<TransformComponent>();
        
        obj->addComponent(xform);
        
        //obj->setActive(true);
        
        return obj;
    }
    
    return nullptr;
}

void BGE::Space::createAutoDisplayObjects(std::shared_ptr<GameObject> root, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
    
    if (package) {
        auto autoDisplayList = package->getAutoDisplayList();
        auto num = package->getAutoDisplayListSize();
        std::map<uint32_t, std::vector<std::shared_ptr<GameObject>>> createdObjects;
        std::vector<std::shared_ptr<GameObject>> rootObjs;
        
        for (auto i=0;i<GfxReferenceTypeLast;i++) {
            createdObjects[i] = std::vector<std::shared_ptr<GameObject>>();
        }
        
        for (auto i=0;i<num;i++) {
            auto elem = &autoDisplayList[i];
            std::shared_ptr<GameObject> obj;
            auto xform = createComponent<TransformComponent>();
            
            if (elem->position) {
                xform->setPosition(*elem->position);
            }
            
            if (elem->scale) {
                xform->setScale(*elem->scale);
            }
            
            xform->setRotation(elem->rotation);
            
            if (elem->colorMatrix) {
                
            }
            
            if (elem->colorTransform) {
                
            }
            
            switch (elem->referenceType) {
                case GfxReferenceTypeAnimationSequence:
                    obj = createAnimSequence(elem->reference);
                    break;
                    
                case GfxReferenceTypeButton:
                    obj = createButton(elem->reference);
                    break;
                
                case GfxReferenceTypeExternalPackage:
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
            
            createdObjects[elem->referenceType].push_back(obj);
            rootObjs.push_back(obj);
        }
        
        for (auto i=0;i<GfxReferenceTypeLast;i++) {
            auto items = createdObjects[i];
            
            if (items.size() > 0) {
                std::function<void(std::shared_ptr<GameObject>)> callback = nullptr;
                
                switch (i) {
                    case GfxReferenceTypeAnimationSequence:
                        callback = delegate.animationSequenceCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeButton:
                        callback = delegate.buttonCreatedHandler;
                        break;
                        
                    case GfxReferenceTypeExternalPackage:
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
                    for (auto item : items) {
                        callback(item);
                    }
                }
            }
        }
        
        for (auto obj : rootObjs) {
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
