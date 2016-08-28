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
#include "ButtonComponent.h"
#include "MaskComponent.h"
#include "SpriteRenderComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"

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

void BGE::Space::destroy() {
    visible_ = false;
    order_ = 0;
    updatable_ = true;
    
    // Destroy all scenePackages
    auto packageService = Game::getInstance()->getScenePackageService();
    
    for (auto const &package : scenePackages_) {
        packageService->removePackage(spaceHandle_, package);
    }
    
    scenePackages_.clear();
    
    // Destroy all game objects
    gameObjectService_->removeAllGameObjects();
    
    // Destroy all components
    componentService_->removeAllComponents();
    
    // Destroy all fonts
    auto fontService = Game::getInstance()->getFontService();
    
    for (auto const &font : fonts_) {
        fontService->removeFont(spaceHandle_, font);
    }
    
    fonts_.clear();
    
    // Destroy all texture atlases
    auto textureService = Game::getInstance()->getTextureService();
    
    for (auto const &atlas : textureAtlases_) {
        textureService->removeTextureAtlas(spaceHandle_, atlas);
    }
    
    textureAtlases_.clear();
    
    // Destory all textures
    for (auto const &texture : textures_) {
        textureService->removeTexture(spaceHandle_, texture);
    }
    
    textures_.clear();
}

void BGE::Space::outputResourceBreakdown(uint32_t numTabs) const {
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Space (%s):\n", getName().c_str());
    
    // Now breakdown resources
    
    // Game objects
    uint32_t num = 0;
    
    for (auto const &objHandle : gameObjectService_->getGameObjects()) {
        auto obj = getGameObject(objHandle);
        
        if (obj) {
            num++;
        }
    }
    
    numTabs++;
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num GameObject: %d\n", num);
    
    num = 0;
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num Component: %d\n", componentService_->totalNumComponents());
    
    componentService_->outputResourceBreakdown(numTabs + 1);

    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num ScenePackage: %ld\n", scenePackages_.size());

    auto scenePackageService = Game::getInstance()->getScenePackageService();
    
    for (auto const &handle : scenePackages_) {
        auto scenePackage = scenePackageService->getScenePackage(handle);
        
        if (scenePackage) {
            scenePackage->outputResourceBreakdown(numTabs + 1);
        }
    }
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num Material: %d\n", componentService_->numMaterials());
    
    auto textureService = Game::getInstance()->getTextureService();
    auto fontService = Game::getInstance()->getFontService();
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num TextureAtlas: %ld\n", textureAtlases_.size());
    
    for (auto const &handle : textureAtlases_) {
        auto atlas = textureService->getTextureAtlas(handle);
        
        if (atlas) {
            for (auto i=0;i<numTabs + 1;i++) {
                printf("\t");
            }
            
            printf("%s\n", atlas->getName().c_str());
        }
    }

    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num Texture: %ld\n", textures_.size());

    for (auto const &handle : textures_) {
        auto texture = textureService->getTexture(handle);
        
        if (texture) {
            for (auto i=0;i<numTabs + 1;i++) {
                printf("\t");
            }
            
            printf("%s\n", texture->getName().c_str());
        }
    }
    
    for (auto i=0;i<numTabs;i++) {
        printf("\t");
    }
    
    printf("Num Font: %ld\n", fonts_.size());

    for (auto const &handle : fonts_) {
        auto font = fontService->getFont(handle);
        
        if (font) {
            for (auto i=0;i<numTabs + 1;i++) {
                printf("\t");
            }
            
            printf("%s\n", font->getNameAsKey().c_str());

        }
    }
}

BGE::GameObject *BGE::Space::createGameObject(std::string name ) {
    return gameObjectService_->createGameObject(name);
}

BGE::GameObjectHandle BGE::Space::getGameObjectHandle(ObjectId objId) const {
    return gameObjectService_->getGameObjectHandle(objId);
}

BGE::GameObjectHandle BGE::Space::getGameObjectHandle(std::string name) const {
    return gameObjectService_->getGameObjectHandle(name);
}

BGE::GameObject *BGE::Space::getGameObject(ObjectId objId) const {
    return gameObjectService_->getGameObject(objId);
}

BGE::GameObject *BGE::Space::getGameObject(std::string name) const {
    return gameObjectService_->getGameObject(name);
}

BGE::GameObject *BGE::Space::getGameObject(GameObjectHandle handle) const {
    return gameObjectService_->getGameObject(handle);
}

void BGE::Space::removeGameObject(GameObjectHandle handle) {
    gameObjectService_->removeGameObject(handle);
}

void BGE::Space::removeGameObject(GameObject *object) {
    gameObjectService_->removeGameObject(object);
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
                    for (auto const &itemHandle : itemHandles) {
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
        
        for (auto const &objHandle : rootObjHandles) {
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

void BGE::Space::createFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback) {
    // TODO: This needs to be thread safe at some point
    
    auto fontService = Game::getInstance()->getFontService();
    auto font = fontService->getFont(name, pxSize);
    auto fontHandle = font->getHandle();
    
    // Now search to see if our handle exists already
    for (auto const &handle : fonts_) {
        if (handle == fontHandle) {
            if (callback) {
                callback(handle, nullptr);
            }
            
            return;
        }
    }
    
    // Not found, so try and allocate it
    Game::getInstance()->getFontService()->createFont(name, pxSize, spaceHandle_, [this, callback](FontHandle font, std::shared_ptr<Error> error) -> void {
        fonts_.push_back(font);
        if (callback) {
            callback(font, error);
        }
    });
}

void BGE::Space::scenePackageAdded(ScenePackageHandle handle) {
    bool found = false;
    
    for (auto const &package : scenePackages_) {
        if (package == handle) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        scenePackages_.push_back(handle);
    }
}

void BGE::Space::scenePackageRemoved(ScenePackageHandle handle) {
    for (auto it=scenePackages_.begin();it!=scenePackages_.end();++it) {
        if (*it == handle) {
            scenePackages_.erase(it);
            return;
        }
    }
}

