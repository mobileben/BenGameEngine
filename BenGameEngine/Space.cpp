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
#include "BoundingBoxComponent.h"
#include "ButtonComponent.h"
#include "MaskComponent.h"
#include "TextureMaskComponent.h"
#include "SpriteRenderComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "PlacementComponent.h"
#include "FlatRectRenderComponent.h"

#include <future>

BGE::Space::Space() : NamedObject(), visible_(false), order_(0), updatable_(true), resetting_(false) {
}

BGE::Space::Space(ObjectId spaceId) : NamedObject(spaceId), visible_(false), order_(0), updatable_(true), resetting_(false) {
}

BGE::Space::Space(ObjectId spaceId, std::string name) : NamedObject(spaceId, name), visible_(false), order_(0), updatable_(true), resetting_(false) {
}

void BGE::Space::initialize(SpaceHandle handle, std::string name, std::shared_ptr<SpaceService> service) {
    spaceHandle_ = handle;
    setName(name);
    spaceService_ = service;

    // By default, spaces are not active or visible or updatable
    active_ = false;
    visible_ = false;
    updatable_ = false;
    resetting_ = false;

    order_ = 0;

    gameObjectService_ = std::make_shared<GameObjectService>();
    gameObjectService_->setSpaceHandle(spaceHandle_);
    
    componentService_ = std::make_shared<ComponentService>();
    componentService_->setSpaceHandle(spaceHandle_);
}

void BGE::Space::destroy() {
    reset([this]() {
        order_ = 0;
        spaceHandle_ = SpaceHandle();
    });
}

void BGE::Space::lock() {
    spaceService_->lock();
}

void BGE::Space::unlock() {
    spaceService_->unlock();
}

void BGE::Space::reset(std::function<void()> callback) {
    lock();
    if (!resetting_) {
        printf("GGGGGG Space %s resetting starting\n", getName().c_str());
        resetting_ = true;
        BGE::Game::getInstance()->getSpaceService()->queueReset(spaceHandle_);
    }
    unlock();
}

void BGE::Space::reset_() {
    // Locking happens externally
    printf("GGGG Space %s ACTUAL reset_ ocurring\n", getName().c_str());
    // Turn everything off just in case this object is still accessible
    visible_ = false;
    updatable_ = false;
    
    // We forcibly remove any handlers. The game should play nice and do the clean up themselves.
    BGE::Game::getInstance()->servicesSpaceReset(this);
    
    // Destroy all scenePackages
    auto packageService = Game::getInstance()->getScenePackageService();
    
    auto package = scenePackages_;
    
    for (auto const &package : package) {
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
    
    // Destroy all textures
    for (auto const &texture : textures_) {
        textureService->removeTexture(spaceHandle_, texture);
    }
    
    // Remove any outstanding events
    
    textures_.clear();
    resetting_ = false;
    printf("GGGGGG Space %s resetting done\n", getName().c_str());
}

uint32_t BGE::Space::handlerBitmaskForSceneObjectCreatedDelegate(SceneObjectCreatedDelegate *delegate) {
    uint32_t bitmask = 0;
    
    if (delegate) {
        if (delegate->animationSequenceCreatedHandler != nullptr) {
            bitmask |= AnimationSequenceComponent::bitmask_;
        }
        
        if (delegate->buttonCreatedHandler != nullptr) {
            bitmask |= ButtonComponent::bitmask_;
        }
        
        if (delegate->maskCreatedHandler != nullptr) {
            bitmask |= MaskComponent::bitmask_;
        }
        
        if (delegate->placementCreatedHandler != nullptr) {
            bitmask |= PlacementComponent::bitmask_;
        }
        
        if (delegate->spriteCreatedHandler != nullptr) {
            bitmask |= SpriteRenderComponent::bitmask_;
        }
        
        if (delegate->textCreatedHandler != nullptr) {
            bitmask |= TextComponent::bitmask_;
        }
        
        if (delegate->textureMaskCreatedHandler != nullptr) {
            bitmask |= TextureMaskComponent::bitmask_;
        }
    }
    
    return bitmask;
}

void BGE::Space::outputResourceBreakdown(uint32_t numTabs) const {
    Game::outputValue(numTabs, "Space (%s):\n", getName().c_str());
    
    // Now breakdown resources
    numTabs++;
    
    // Game objects
    uint32_t num = 0;
    
    for (auto const &objHandle : gameObjectService_->getGameObjects()) {
        auto obj = getGameObject(objHandle);
        
        if (obj) {
            num++;
        }
    }
    
    Game::outputValue(numTabs, "Num GameObject: %u\n", num);
    
    Game::outputValue(numTabs, "Num Component: %u\n", componentService_->totalNumComponents());
    
    componentService_->outputResourceBreakdown(numTabs + 1);
    
    Game::outputValue(numTabs, "Num ScenePackage: %zd\n", scenePackages_.size());

    auto scenePackageService = Game::getInstance()->getScenePackageService();
    
    for (auto const &handle : scenePackages_) {
        auto scenePackage = scenePackageService->getScenePackage(handle);
        
        if (scenePackage) {
            scenePackage->outputResourceBreakdown(numTabs + 1);
        }
    }
    
    Game::outputValue(numTabs, "Num Material: %d\n", componentService_->numMaterials());
    
    auto textureService = Game::getInstance()->getTextureService();
    auto fontService = Game::getInstance()->getFontService();
    
    Game::outputValue(numTabs, "Num TextureAtlas: %zd\n", textureAtlases_.size());
    
    for (auto const &handle : textureAtlases_) {
        auto atlas = textureService->getTextureAtlas(handle);
        
        if (atlas) {
            Game::outputValue(numTabs, "%s\n", atlas->getName().c_str());
        }
    }

    Game::outputValue(numTabs, "Num Texture: %zd\n", textures_.size());

    for (auto const &handle : textures_) {
        auto texture = textureService->getTexture(handle);
        
        if (texture) {
            Game::outputValue(numTabs, "%s\n", texture->getName().c_str());
        }
    }
    
    Game::outputValue(numTabs, "Num Font: %zd\n", fonts_.size());

    for (auto const &handle : fonts_) {
        auto font = fontService->getFont(handle);
        
        if (font) {
            Game::outputValue(numTabs, "%s\n", font->getNameAsKey().c_str());
        }
    }
}

void BGE::Space::outputMemoryBreakdown(uint32_t numTabs) const {
    Game::outputValue(numTabs, "Space (%s):\n", getName().c_str());
    
    // Now breakdown resources
    numTabs++;
    
    componentService_->outputMemoryBreakdown(numTabs);
}

BGE::GameObject *BGE::Space::createGameObject(std::string name ) {
    lock();
    
    auto obj = gameObjectService_->createGameObject(name);
    
    unlock();
    
    return obj;
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
    lock();
    gameObjectService_->removeGameObject(handle);
    unlock();
}

void BGE::Space::removeGameObject(GameObject *object) {
    lock();
    gameObjectService_->removeGameObject(object);
    unlock();
}

const std::vector<BGE::GameObjectHandle>& BGE::Space::getGameObjects() const {
    return gameObjectService_->getGameObjects();
}

void BGE::Space::getRootGameObjects(std::vector<GameObject *> &objects) {
    auto handles = getGameObjects();
    
    for (auto const &handle : handles) {
        auto obj = getGameObject(handle);
        // Only active root game objects can be returned
        if (obj && obj->isActive()) {
            auto xform = obj->getComponent<TransformComponent>();

            if (xform) {
                if (!xform->getParent()) {
                    objects.push_back(obj);
                }
            }
        }
    }
}

void BGE::Space::getReverseRootGameObjects(std::vector<GameObject *> &objects) {
    auto handles = getGameObjects();
    
    for (int32_t i=static_cast<int32_t>(handles.size())-1;i>=0;--i) {
        auto obj = getGameObject(handles[i]);
        // Only active root game objects can be returned
        if (obj && obj->isActive()) {
            auto xform = obj->getComponent<TransformComponent>();

            if (xform) {
                if (!xform->getParent()) {
                    objects.push_back(obj);
                }
            }
        }
    }
}

void BGE::Space::getTransforms(std::vector<TransformComponent *> &xforms) const {
    componentService_->getComponents<TransformComponent>(xforms);
}

void BGE::Space::getRootTransforms(std::vector<TransformComponent *> &xforms) const {
    auto handleService = componentService_->getComponentHandleService<TransformComponent>();
    auto const &handles = componentService_->getComponentHandles<TransformComponent>();
    
    for (auto const &handle : handles) {
        auto xform = handleService->dereference(TransformComponentHandle(handle.handle));
        
        if ( xform->getParent() == nullptr) {
            xforms.push_back(xform);
        }
    }

    componentService_->getComponents<TransformComponent>(xforms);
}

void BGE::Space::loadAllTextures(std::function<void()> callback) {
    auto scenePackageService = Game::getInstance()->getScenePackageService();

    for (auto &packageHandle : scenePackages_) {
        auto package = scenePackageService->getScenePackage(packageHandle);

        if (package) {
            package->link();
        }
    }
}

// TODO: Add in support for dependency validation as well as linking in order of dependencies
void BGE::Space::linkAll() {
    auto scenePackageService = Game::getInstance()->getScenePackageService();
    
    for (auto &packageHandle : scenePackages_) {
        auto package = scenePackageService->getScenePackage(packageHandle);
        
        if (package) {
            package->link();
        }
    }
}

BGE::GameObject *BGE::Space::createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate) {
    lock();
    
    CreatedGameObjectVector objects;
    auto bitmask = Space::handlerBitmaskForSceneObjectCreatedDelegate(delegate);
    auto obj = createAnimSequence(name, instanceName, handle, bitmask, &objects);
    
    dispatchCreatedHandlers(&objects, delegate);
    
    unlock();
    
    return obj;
}

BGE::GameObject *BGE::Space::createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        lock();

        auto obj = createGameObject(instanceName);
        auto objHandle = obj->getHandle();
        auto xform = createComponent<TransformComponent>();
        auto boundingBox = createComponent<BoundingBoxComponent>();

        auto animSeq = createComponent<AnimationSequenceComponent>();
        auto animator = createComponent<AnimatorComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(boundingBox);
        obj->addComponent(animSeq);
        obj->addComponent(animator);
        
        setAnimationSequenceReference(animSeq->getHandle<AnimationSequenceComponent>(), animSeqRef);
        
        // Refresh obj/animator in case handle capacity increased
        obj = getGameObject(objHandle);
        animator = obj->getComponent<AnimatorComponent>();
        
        animator->setFrame(0, true);
        
        addCreatedGameObjectsForAnimSequence(obj, pushBitmask, objects);
        
        unlock();
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createAnimChannel(std::string name, std::string instanceName, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate *delegate) {
    if (channelRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto objHandle = obj->getHandle();
        auto xform = createComponent<TransformComponent>();
        auto channel = createComponent<AnimationChannelComponent>();
        auto animator = createComponent<ChannelFrameAnimatorComponent>();

        assert(obj);
        
        obj->addComponent(xform);
        obj->addComponent(channel);
        obj->addComponent(animator);
        
        setAnimationChannelReference(channel->getHandle<AnimationChannelComponent>(), channelRef);
        
        // Refresh obj/animator in case handle capacity increased
        obj = getGameObject(objHandle);

        unlock();

        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createFrameAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    AnimationSequenceReference *animSeqRef;
    
    if (package) {
        animSeqRef = package->getAnimationSequenceReference(name);
    } else {
        animSeqRef = Game::getInstance()->getScenePackageService()->getAnimationSequenceReference(name);
    }
    
    if (animSeqRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto objHandle = obj->getHandle();
        auto xform = createComponent<TransformComponent>();

        auto animSeq = createComponent<AnimationSequenceComponent>();
        auto animator = createComponent<FrameAnimatorComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(animSeq);
        obj->addComponent(animator);
        
        setAnimationSequenceReference(animSeq->getHandle<AnimationSequenceComponent>(), animSeqRef);
        
        // Refresh obj/animator in case handle capacity increased
        obj = getGameObject(objHandle);
        
        unlock();

        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createButton(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate) {
    lock();

    CreatedGameObjectVector objects;
    
    auto bitmask = Space::handlerBitmaskForSceneObjectCreatedDelegate(delegate);
    auto obj = createButton(name, instanceName, handle, bitmask, &objects);
    
    dispatchCreatedHandlers(&objects, delegate);

    unlock();

    return obj;
}

BGE::GameObject *BGE::Space::createButton(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    ButtonReference *buttonRef;
    
    if (package) {
        buttonRef = package->getButtonReference(name);
    } else {
        buttonRef = Game::getInstance()->getScenePackageService()->getButtonReference(name);
    }
    
    if (buttonRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto objHandle = obj->getHandle();
        auto xform = createComponent<TransformComponent>();
        auto button = createComponent<ButtonComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(button);
        obj->addComponent(bbox);
        
        button->setButtonReference(buttonRef);
        
        // Refresh obj/animator in case handle capacity increased
        obj = getGameObject(objHandle);
        
        addCreatedGameObjectsForButton(obj, pushBitmask, objects);

        unlock();

        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate) {
    lock();

    CreatedGameObjectVector objects;
    uint32_t pushBitmask = Space::handlerBitmaskForSceneObjectCreatedDelegate(delegate);
    auto obj = createExternalReference(name, instanceName, handle, pushBitmask, &objects);
    
    dispatchCreatedHandlers(&objects, delegate);
    
    unlock();

    return obj;
}

BGE::GameObject *BGE::Space::createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
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
            
            switch (type) {
                case GfxReferenceTypeAnimationSequence:
                    return createAnimSequence(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeButton:
                    return createButton(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeExternalReference:
                    return createExternalReference(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeMask:
                    return createMask(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypePlacement:
                    return createPlacement(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeSprite:
                    return createSprite(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeText:
                    return createText(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                case GfxReferenceTypeTextureMask:
                    return createTextureMask(refName, instanceName, extPackage->getHandle(), pushBitmask, objects);
                    
                default:
                    assert(false);
                    break;
            }
        }
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createMask(std::string name, std::string instanceName, ScenePackageHandle handle) {
    return createMask(name, instanceName, handle, 0, nullptr);
}

BGE::GameObject *BGE::Space::createMask(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    MaskReference *maskRef;
    
    if (package) {
        maskRef = package->getMaskReference(name);
    } else {
        maskRef = Game::getInstance()->getScenePackageService()->getMaskReference(name);
    }
    
    if (maskRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto xform = createComponent<TransformComponent>();
        auto mask = createComponent<MaskComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(mask);
        
        mask->setMaskReference(maskRef);
        
        if (objects && pushBitmask & MaskComponent::bitmask_) {
            addCreatedGameObjectsForRenderComponent<MaskComponent>(obj, objects);
        }

        unlock();
        
        return obj;
    }
    
    return nullptr;

}

BGE::GameObject *BGE::Space::createTextureMask(std::string name, std::string instanceName, ScenePackageHandle handle) {
    return createTextureMask(name, instanceName, handle, 0, nullptr);
}

BGE::GameObject *BGE::Space::createTextureMask(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextureMaskReference *maskRef;
    
    if (package) {
        maskRef = package->getTextureMaskReference(name);
    } else {
        maskRef = Game::getInstance()->getScenePackageService()->getTextureMaskReference(name);
    }
    
    if (maskRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto xform = createComponent<TransformComponent>();
        auto mask = createComponent<TextureMaskComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(mask);
        
        mask->setTextureMaskReference(maskRef);
        
        if (objects && pushBitmask & TextureMaskComponent::bitmask_) {
            addCreatedGameObjectsForRenderComponent<TextureMaskComponent>(obj, objects);
        }
        
        unlock();
        
        return obj;
    }
    
    return nullptr;
    
}

BGE::GameObject *BGE::Space::createSprite(std::string name, std::string instanceName, ScenePackageHandle handle) {
    return createSprite(name, instanceName, handle, 0, nullptr);
}

BGE::GameObject *BGE::Space::createSprite(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextureReference *texRef;
    
    if (package) {
        texRef = package->getTextureReference(name);
    } else {
        texRef = Game::getInstance()->getScenePackageService()->getTextureReference(name);
    }
    
    if (texRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto xform = createComponent<TransformComponent>();
        auto sprite = createComponent<SpriteRenderComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(sprite);
        obj->addComponent(bbox);
        
        sprite->setTextureReference(texRef);
        
        if (objects && pushBitmask & SpriteRenderComponent::bitmask_) {
            addCreatedGameObjectsForRenderComponent<SpriteRenderComponent>(obj, objects);
        }
        
        unlock();
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createText(std::string name, std::string instanceName, ScenePackageHandle handle) {
    return createText(name, instanceName, handle, 0, nullptr);
}

BGE::GameObject *BGE::Space::createText(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    TextReference *textRef;
    
    if (package) {
        textRef = package->getTextReference(name);
    } else {
        textRef = Game::getInstance()->getScenePackageService()->getTextReference(name);
    }
    
    if (textRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto xform = createComponent<TransformComponent>();
        auto text = createComponent<TextComponent>();
        auto bbox = createComponent<BoundingBoxComponent>();
        
        obj->addComponent(xform);
        obj->addComponent(text);
        obj->addComponent(bbox);
        
        text->setTextReference(textRef);
        
        if (objects && pushBitmask & TextComponent::bitmask_) {
            addCreatedGameObjectsForRenderComponent<TextComponent>(obj, objects);
        }
        
        unlock();
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle) {
    return createPlacement(name, instanceName, handle, 0, nullptr);
}

BGE::GameObject *BGE::Space::createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(handle);
    PlacementReference *placementRef;
    
    if (package) {
        placementRef = package->getPlacementReference(name);
    } else {
        placementRef = Game::getInstance()->getScenePackageService()->getPlacementReference(name);
    }
    
    if (placementRef) {
        lock();
        
        auto obj = createGameObject(instanceName);
        auto xform = createComponent<TransformComponent>();
        auto placement = createComponent<PlacementComponent>();
        
        obj->addComponent(xform);
        
        placement->setPlacementReference(placementRef);
        
        obj->addComponent(placement);
        
        if (objects && pushBitmask & PlacementComponent::bitmask_) {
            addCreatedGameObjectsForRenderComponent<PlacementComponent>(obj, objects);
        }
        
        unlock();
        
        return obj;
    }
    
    return nullptr;
}

BGE::GameObject *BGE::Space::createFlatRect(std::string instanceName, Vector2 &wh, Color &color) {
    lock();
    
    auto obj = createGameObject(instanceName);
    auto xform = createComponent<TransformComponent>();
    auto flat = createComponent<FlatRectRenderComponent>();
    auto material = Game::getInstance()->getMaterialService()->createMaterial(color);
    auto bbox = createComponent<BoundingBoxComponent>();
    
    bbox->x = 0;
    bbox->y = 0;
    bbox->width = wh.w;
    bbox->height = wh.h;
    
    flat->setWidthHeight(wh);
    flat->setMaterials({material});
    
    obj->addComponent(xform);
    obj->addComponent(flat);
    obj->addComponent(bbox);
    
    unlock();
    
    return obj;
}

BGE::GameObject *BGE::Space::createSprite(std::string instanceName, TextureHandle texHandle) {
    if (texHandle.isNull()) {
        return nullptr;
    }
    
    lock();
    
    auto obj = createGameObject(instanceName);
    auto xform = createComponent<TransformComponent>();
    auto sprite = createComponent<SpriteRenderComponent>();
    auto bbox = createComponent<BoundingBoxComponent>();

    obj->addComponent(xform);
    obj->addComponent(sprite);
    obj->addComponent(bbox);

    sprite->setTextureHandle(texHandle);
    
    unlock();
    
    return obj;
}

BGE::GameObject *BGE::Space::createSprite(std::string instanceName, Texture *texture) {
    if (!texture) {
        return nullptr;
    }
    
    lock();
    
    auto obj = createGameObject(instanceName);
    auto xform = createComponent<TransformComponent>();
    auto sprite = createComponent<SpriteRenderComponent>();
    auto bbox = createComponent<BoundingBoxComponent>();
    
    obj->addComponent(xform);
    obj->addComponent(sprite);
    obj->addComponent(bbox);
    
    sprite->setTexture(texture);
    
    unlock();
    
    return obj;
}

void BGE::Space::createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate *delegate, std::function<void()> callback) {
    auto f = std::async(std::launch::async, static_cast<void(Space::*)(GameObjectHandle, ScenePackageHandle, SceneObjectCreatedDelegate *, std::function<void()>)>(&Space::createAutoDisplayObjectsSynchronous), this, rootHandle, packageHandle, delegate, callback);
}

void BGE::Space::createAutoDisplayObjectsSynchronous(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate *delegate, std::function<void()> callback) {
    auto package = Game::getInstance()->getScenePackageService()->getScenePackage(packageHandle);
    auto bitmask = Space::handlerBitmaskForSceneObjectCreatedDelegate(delegate);
    auto animationService = Game::getInstance()->getAnimationService();
    
    if (package) {
        animationService->lock();
        lock();
        
        auto autoDisplayList = package->getAutoDisplayList();
        auto num = package->getAutoDisplayListSize();
        CreatedGameObjectVector createdObjects;
        std::vector<GameObjectHandle> rootObjHandles;
        
        for (auto i=0;i<num;i++) {
            auto elem = &autoDisplayList[i];
            GameObject *obj;
            
            switch (elem->referenceType) {
                case GfxReferenceTypeAnimationSequence:
                    obj = createAnimSequence(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypeButton:
                    obj = createButton(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                
                case GfxReferenceTypeExternalReference:
                    obj = createExternalReference(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypeMask:
                    obj = createMask(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypePlacement:
                    obj = createPlacement(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypeSprite:
                    obj = createSprite(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypeText:
                    obj = createText(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                case GfxReferenceTypeTextureMask:
                    obj = createTextureMask(elem->reference, elem->name, packageHandle, bitmask, &createdObjects);
                    break;
                    
                default:
                    assert(false);
                    break;
            }
            
            if (obj) {
                auto xform = obj->getComponent<TransformComponent>();
                
                if (elem->position) {
                    xform->setPosition(*elem->position);
                }
                
                if (elem->scale) {
                    xform->setScale(*elem->scale);
                }
                
                xform->setRotationInDegrees(elem->rotation);
                
                if (elem->colorMatrix) {
                    
                }
                
                if (elem->colorTransform) {
                    
                }
                
                if (elem->hidden) {
                    xform->setVisibility(false);
                }
                
                rootObjHandles.push_back(obj->getHandle());
            }
        }
        
        TransformComponent *rootXform = nullptr;
        
        // Update root in case allocations moved handles
        auto root = getGameObject(rootHandle);
        
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

        // Now notifiy all create handlers
        dispatchCreatedHandlers(&createdObjects, delegate);
        unlock();
        animationService->unlock();
    }
    
    if (callback) {
        callback();
    }
}

std::pair<BGE::FontHandle, std::shared_ptr<BGE::Error>> BGE::Space::createFont(std::string name, uint32_t pxSize) {
    lock();
    
    auto fontService = Game::getInstance()->getFontService();
    auto font = fontService->getFont(name, pxSize);
    if (font) {
        auto fontHandle = font->getHandle();
        // Now search to see if our handle exists already
        for (auto const &handle : fonts_) {
            if (handle == fontHandle) {
                unlock();
                return std::make_pair(handle, nullptr);
            }
        }
    }

    // Not found, so try and allocate it
    FontHandle fontHandle;
    std::shared_ptr<Error> error;
    std::tie(fontHandle, error) = Game::getInstance()->getFontService()->createFont(name, pxSize, spaceHandle_);
    if (!fontHandle.isNull()) {
        fonts_.push_back(fontHandle);
    }

    unlock();
    return std::make_pair(fontHandle, error);
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

BGE::GameObjectHandle BGE::Space::handleForGameObject(GameObject *object) const {
    if (object) {
        return object->getHandle();
    } else {
        return GameObjectHandle();
    }
}

void BGE::Space::addCreatedGameObjectsForAnimSequence(GameObject *animSequence, uint32_t pushBitmask, CreatedGameObjectVector *objects) const {
    if (!pushBitmask) {
        return;
    }
    
    if (animSequence && objects) {
        if (pushBitmask & AnimationSequenceComponent::bitmask_) {
            // Animation sequence always pushed first
            objects->push_back(std::make_pair(AnimationSequenceComponent::typeId_, animSequence->getHandle()));
        }
        
        auto animSeqComponent = animSequence->getComponent<AnimationSequenceComponent>();
        auto pushSprites = pushBitmask & SpriteRenderComponent::bitmask_;
        auto pushButtons = pushBitmask & ButtonComponent::bitmask_;
        auto pushAnims = pushBitmask & AnimationSequenceComponent::bitmask_;
        auto pushText = pushBitmask & TextComponent::bitmask_;
        auto pushMasks = pushBitmask & MaskComponent::bitmask_;
        auto pushPlacements = pushBitmask & PlacementComponent::bitmask_;
        auto pushTextureMasks = pushBitmask & TextureMaskComponent::bitmask_;
        
        if (animSeqComponent) {
            for (auto &channel : animSeqComponent->channels) {
                auto channelObj = getGameObject(channel);
                
                if (channelObj) {
                    auto channelComponent = channelObj->getComponent<AnimationChannelComponent>();
                    
                    if (channelComponent) {
                        switch (channelComponent->channel->referenceType) {
                            case GfxReferenceTypeSprite: {
                                auto sprite = channelObj->getComponent<SpriteRenderComponent>();
                                
                                if (sprite) {
                                    if (pushSprites) {
                                        objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeButton:{
                                auto button = channelObj->getComponent<ButtonComponent>();
                                
                                if (button) {
                                    if (pushButtons) {
                                        objects->push_back(std::make_pair(ButtonComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeKeyframe: {
                                // Keyframe has sequence on child
                                auto xform = channelObj->getComponent<TransformComponent>();
                                auto childXform = xform->childAtIndex(0);
                                
                                if (childXform) {
                                    auto child = childXform->getGameObject();
                                    if (child) {
                                        auto animSeq = child->getComponent<AnimationSequenceComponent>();
                                        
                                        if (animSeq) {
                                            if (pushAnims) {
                                                objects->push_back(std::make_pair(AnimationSequenceComponent::typeId_, channelObj->getHandle()));
                                            }
                                            
                                            addCreatedGameObjectsForAnimSequence(child, pushBitmask, objects);
                                        } else {
                                            assert(false);
                                        }
                                    }
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeAnimationSequence: {
                                auto animSeq = channelObj->getComponent<AnimationSequenceComponent>();
                                
                                if (animSeq) {
                                    if (pushAnims) {
                                        objects->push_back(std::make_pair(AnimationSequenceComponent::typeId_, channelObj->getHandle()));
                                    }
                                    
                                    addCreatedGameObjectsForAnimSequence(channelObj, pushBitmask, objects);
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeMask: {
                                auto mask = channelObj->getComponent<MaskComponent>();
                                
                                if (mask) {
                                    if (pushMasks) {
                                        objects->push_back(std::make_pair(MaskComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeText: {
                                auto text = channelObj->getComponent<TextComponent>();
                                
                                if (text) {
                                    if (pushText) {
                                        objects->push_back(std::make_pair(TextComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypePlacement: {
                                auto placement = channelObj->getComponent<PlacementComponent>();
                                
                                if (placement) {
                                    if (pushPlacements) {
                                        objects->push_back(std::make_pair(PlacementComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;
                                
                            case GfxReferenceTypeTextureMask: {
                                auto mask = channelObj->getComponent<TextureMaskComponent>();
                                
                                if (mask) {
                                    if (pushMasks) {
                                        objects->push_back(std::make_pair(TextureMaskComponent::typeId_, channelObj->getHandle()));
                                    }
                                } else {
                                    assert(false);
                                }
                            }
                                break;

                            default:
                                break;
                        }
                    } else {
                        assert(false);
                    }
                }
            }
        } else {
            assert(false);
        }
    }
}

void BGE::Space::addCreatedGameObjectsForButton(GameObject *button, uint32_t pushBitmask, CreatedGameObjectVector *objects) const {
    if (!pushBitmask) {
        return;
    }
    
    if (button && objects) {
        if (pushBitmask & ButtonComponent::bitmask_) {
            // Button is always pushed first
            objects->push_back(std::make_pair(ButtonComponent::typeId_, button->getHandle()));
        }
        
        auto pushSprites = pushBitmask & SpriteRenderComponent::bitmask_;
        auto pushAnims = pushBitmask & AnimationSequenceComponent::bitmask_;
        
        auto buttonComponent = button->getComponent<ButtonComponent>();
        
        auto state = getGameObject(buttonComponent->disabledButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }
        
        state = getGameObject(buttonComponent->disabledAnimButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }
        
        state = getGameObject(buttonComponent->normalButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }

        state = getGameObject(buttonComponent->normalAnimButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }

        state = getGameObject(buttonComponent->highlightedButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }

        state = getGameObject(buttonComponent->highlightedAnimButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }

        state = getGameObject(buttonComponent->currentButtonHandle);
        
        if (state) {
            if (state->hasComponent<SpriteRenderComponent>()) {
                if (pushSprites) {
                    objects->push_back(std::make_pair(SpriteRenderComponent::typeId_, state->getHandle()));
                }
            } else {
                // Must be AnimationSequenceComponent
                if (pushAnims) {
                    addCreatedGameObjectsForAnimSequence(state, pushBitmask, objects);
                }
            }
        }
    }
}

void BGE::Space::dispatchCreatedHandlers(CreatedGameObjectVector *objects, SceneObjectCreatedDelegate *delegate) {
    if (delegate) {
        if (objects) {
            for (auto &created : *objects) {
                if (created.first == AnimationSequenceComponent::typeId_) {
                    delegate->animationSequenceCreatedHandler(getGameObject(created.second));
                } else if (created.first == ButtonComponent::typeId_) {
                    delegate->buttonCreatedHandler(getGameObject(created.second));
                } else if (created.first == SpriteRenderComponent::typeId_) {
                    delegate->spriteCreatedHandler(getGameObject(created.second));
                } else if (created.first == TextComponent::typeId_) {
                    delegate->textCreatedHandler(getGameObject(created.second));
                } else if (created.first == PlacementComponent::typeId_) {
                    delegate->placementCreatedHandler(getGameObject(created.second));
                } else if (created.first == MaskComponent::typeId_) {
                    delegate->maskCreatedHandler(getGameObject(created.second));
                } else if (created.first == TextureMaskComponent::typeId_) {
                    delegate->textureMaskCreatedHandler(getGameObject(created.second));
                }
            }
        }
    }
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::Space::createTextureFromFile(std::string name, std::string filename, TextureFormat format) {
    return Game::getInstance()->getTextureService()->createTextureFromFile(spaceHandle_, name, filename, format);
}

std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::Space::createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height) {
    return Game::getInstance()->getTextureService()->createTextureFromBuffer(spaceHandle_, name, buffer, format, width, height);
}

#if TARGET_OS_IPHONE
std::pair<BGE::Texture *, std::shared_ptr<BGE::Error>> BGE::Space::createTextureFromUIImage(const std::string& name, UIImage *image) {
    return Game::getInstance()->getTextureService()->createTextureFromUIImage(spaceHandle_, name, image);
}
#endif /* TARGET_OS_IPHONE */

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::Space::createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format) {
    return Game::getInstance()->getTextureService()->createTextureAtlasFromFile(spaceHandle_, name, filename, subTextureDefs, format);
}

std::pair<BGE::TextureAtlas *, std::shared_ptr<BGE::Error>> BGE::Space::createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs) {
    return Game::getInstance()->getTextureService()->createTextureAtlasFromBuffer(spaceHandle_, name, buffer, format, width, height, subTextureDefs);
}

BGE::TextureHandle BGE::Space::getTextureHandle(std::string name) const {
    return Game::getInstance()->getTextureService()->getTextureHandle(spaceHandle_, name);
}

BGE::TextureAtlasHandle BGE::Space::getTextureAtlasHandle(std::string name) const {
    return Game::getInstance()->getTextureService()->getTextureAtlasHandle(spaceHandle_, name);
}

BGE::Texture *BGE::Space::getTexture(std::string name) const {
    return Game::getInstance()->getTextureService()->getTexture(spaceHandle_, name);
}

BGE::TextureAtlas *BGE::Space::getTextureAtlas(std::string name) const {
    return Game::getInstance()->getTextureService()->getTextureAtlas(spaceHandle_, name);
}

void BGE::Space::removeTexture(TextureHandle handle) {
    Game::getInstance()->getTextureService()->removeTexture(spaceHandle_, handle);
}

void BGE::Space::removeTextureAtlas(TextureAtlasHandle handle) {
    Game::getInstance()->getTextureService()->removeTextureAtlas(spaceHandle_, handle);
}

void BGE::Space::setAnimationSequenceReference(AnimationSequenceComponentHandle animSeqHandle, AnimationSequenceReference *animSeqRef) {
    if (animSeqRef) {
        setAnimationSequenceReference(animSeqHandle, *animSeqRef);
    }
}

void BGE::Space::setAnimationSequenceReference(AnimationSequenceComponentHandle animSeqHandle, const AnimationSequenceReference& animSeqRef) {
    auto animSeq = getComponent(animSeqHandle);
    
    animSeq->frameRate = animSeqRef.frameRate;
    animSeq->totalFrames = animSeqRef.totalFrames;
    animSeq->numChannels = animSeqRef.numChannels;
    animSeq->numBounds = animSeqRef.numBounds;
    
#ifdef NOT_YET
    NSLog(@"setAnimationSequenceReference");
#endif
    auto root = animSeq->getGameObject();
    auto seqXform = root->getComponent<TransformComponent>();
    
    if (seqXform) {
        seqXform->removeAllChildren();
    }
    
    AnimationChannelReference *channels = animSeqRef.channels;
    auto numChannels = animSeq->numChannels;
    
    for (auto i=0;i<numChannels;i++) {
        auto channelRef = &channels[i];
        auto obj = this->createAnimChannel(channelRef->reference, channelRef->name, channelRef, nullptr);
        auto xform = obj->getComponent<TransformComponent>();
        auto xformHandle = xform->getHandle<TransformComponent>();
        
        // Refresh root/seqXform in case handle capacity changed
        animSeq = getComponent(animSeqHandle);
        root = animSeq->getGameObject();
        seqXform = root->getComponent<TransformComponent>();
        
        seqXform->addChild(xform);
        
        // TODO: Destroy components when done!
        
        animSeq->channels.push_back(obj->getHandle());
        obj->setActive(true);
        
        // If this is a sequence keyframe, we need to create more game objects!
        if (channelRef->referenceType == GfxReferenceTypeKeyframe) {
#ifdef NOT_YET
            NSLog(@"Adding sequence %s/%s to channel", channelRef->name, channelRef->reference);
#endif
            auto newObj = this->createFrameAnimSequence(channelRef->reference, channelRef->name, animSeqRef.scenePackage, nullptr);
            
            if (newObj) {
                auto newXform = newObj->getComponent<TransformComponent>();
                
                // Force update xform in case a resize of handle capacity occurred
                xform = this->getComponent(xformHandle);
                
                xform->addChild(newXform);
                
                newObj->setActive(true);
            }
        }
    }
    
    animSeq = getComponent(animSeqHandle);
    animSeq->bounds = animSeqRef.bounds;
}

void BGE::Space::setAnimationChannelReference(AnimationChannelComponentHandle channelHandle, const AnimationChannelReference *animChanRef) {
    auto channel = getComponent(channelHandle);
    
    if (channel) {
        channel->channel = animChanRef;

        auto gameObj = getGameObject(channel->getGameObjectHandle());
        
        if (channel->channel) {
            auto package = Game::getInstance()->getScenePackageService()->getScenePackage(channel->channel->scenePackage);
            
            // Now setup the proper render component
            
#ifdef NOT_YET
            // TODO: Do we do this later?
            NSLog(@"Channel reference %s/%d", channel->channel->reference, channel->channel->referenceType);
#endif
            switch (channel->channel->referenceType) {
                case GfxReferenceTypeSprite: {
                    auto sprite = createComponent<SpriteRenderComponent>();
                    auto texRef = package->getTextureReference(channel->channel->reference);
                    auto bbox = createComponent<BoundingBoxComponent>();
                    
                    // TODO: Remove any older render components
                    gameObj->addComponent(sprite);
                    gameObj->addComponent(bbox);
                    
                    sprite->setTextureReference(texRef);
                }
                    break;
                    
                case GfxReferenceTypeButton:{
                    auto button = createComponent<ButtonComponent>();
                    auto bbox = createComponent<BoundingBoxComponent>();
                    
                    auto buttonRef = package->getButtonReference(channel->channel->reference);
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
                    auto mask = createComponent<MaskComponent>();
                    auto maskRef = package->getMaskReference(channel->channel->reference);
                    
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
                    TextReference *textRef = package->getTextReference(channel->channel->reference);
                    
                    if (textRef && !textRef->fontHandle.isNull()) {
                        auto text = createComponent<TextComponent>();
                        auto bbox = createComponent<BoundingBoxComponent>();
                        
                        gameObj->addComponent(text);
                        gameObj->addComponent(bbox);
                        
                        text->setTextReference(*textRef);
                    }
                    // TODO: Remove any older render components
                }
                    break;
                    
                case GfxReferenceTypePlacement: {
                    PlacementReference *placementRef = package->getPlacementReference(channel->channel->reference);
                    
                    if (placementRef) {
                        auto placement = createComponent<PlacementComponent>();
                        
                        gameObj->addComponent(placement);
                        
                        placement->setPlacementReference(placementRef);
                    }
                }
                    break;
                    
                case GfxReferenceTypeTextureMask: {
                    auto mask = createComponent<TextureMaskComponent>();
                    auto maskRef = package->getTextureMaskReference(channel->channel->reference);
                    
                    gameObj->addComponent(mask);
                    mask->setTextureMaskReference(maskRef);
                }
                    break;
                    
                default:
                    assert(false);
                    break;
            }
        } else {
#ifdef NOT_YET
            if (channel->channel) {
                NSLog(@"You fail %lx %lx %lx", (unsigned long) gameObj, (unsigned long) channel->channel, (unsigned long) channel->channel->referenceData);
            } else {
                NSLog(@"You fail %lx", (unsigned long)  gameObj);
            }
#endif
        }
    }
}
void BGE::Space::setGameObjectHandle(ComponentHandle compHandle, GameObjectHandle gameObjHandle) {
    if (compHandle.typeId == AnimationChannelComponent::typeId_) {
        auto handle = AnimationChannelComponentHandle(compHandle.handle);
        auto comp = getComponent(handle);
        
        if (comp) {
            setAnimationChannelReference(handle, comp->channel);
        }
    }
}


