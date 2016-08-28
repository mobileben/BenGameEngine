//
//  Game.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Game.h"

std::shared_ptr<BGE::Game> BGE::Game::getInstance() {
    static std::shared_ptr<Game> instance = std::make_shared<Game>();
    return instance;
}

BGE::Game::Game()
{
    materialService_ = std::make_shared<MaterialService>();
    heartbeatService_ = std::make_shared<HeartbeatService>();
    scenePackageService_ = std::make_shared<ScenePackageService>();
    animationService_ = std::make_shared<AnimationService>();
    spaceService_ = std::make_shared<SpaceService>();
    fontService_ = std::make_shared<FontService>();
    inputService_ = std::make_shared<InputService>();
    eventService_ = std::make_shared<EventService>();
}

void BGE::Game::provide(std::shared_ptr<RenderService> renderService) {
    renderService_ = renderService;
}

void BGE::Game::provide(std::shared_ptr<TextureService> textureService) {
    textureService_ = textureService;
}

void BGE::Game::initialize() {
    // Create these here because they rely on statics and cannot be created during cxx_global_var_init
    componentService_ = std::make_shared<ComponentService>();
    gameObjectService_ = std::make_shared<GameObjectService>();
    
    // Must call this during initialize since doing it during ctor involves statics from other classes not being
    ComponentService::registerComponents();
    
    getHeartbeatService()->registerListener("Game", std::bind(&Game::update, this, std::placeholders::_1), 0);
}

void BGE::Game::reset() {
}

void BGE::Game::enteringBackground() {
}

void BGE::Game::enteringForeground() {
}

void BGE::Game::pause() {
}

void BGE::Game::resume() {
}

void BGE::Game::destroy() {
}

void BGE::Game::update(double deltaTime) {
    NSLog(@"delta time %f", deltaTime);
    inputService_->process();
    animationService_->update(deltaTime);
}

void BGE::Game::outputResourceUsage() const {
    printf("\n-------- Resource Usage START --------\n");
    printf("Num GameObject: %d\n", gameObjectService_->numGameObjects());
    printf("Num Component: %d\n", componentService_->totalNumComponents());
    componentService_->outputResourceBreakdown(1);
    printf("Num Space: %d\n", spaceService_->numSpaces());
    spaceService_->outputResourceBreakdown(1);
    printf("Num ScenePackage: %d\n", scenePackageService_->numScenePackages());
    printf("Num Material: %d\n", materialService_->numMaterials());
    printf("Num Font: %d\n", fontService_->numFonts());
    printf("Num TextureAtlas: %d\n", textureService_->numTextureAtlases());
    printf("Num Texture: %d\n", textureService_->numTextures());
    printf("Num Texture (sub): %d\n", textureService_->numSubTextures());
    printf("-------- Resource Usage END --------\n\n");
    
}

void BGE::Game::outputResourceBreakdown(uint32_t numTabs) const {
    printf("\n-------- Resource Breakdown START --------\n");
    printf("Num GameObject: %d\n", gameObjectService_->numGameObjects());
    printf("Num Component: %d\n", componentService_->totalNumComponents());
    componentService_->outputResourceBreakdown(numTabs + 1);
    printf("Num Space: %d\n", spaceService_->numSpaces());
    spaceService_->outputResourceBreakdown(numTabs + 1);
    printf("Num ScenePackage: %d\n", scenePackageService_->numScenePackages());
    printf("Num Material: %d\n", materialService_->numMaterials());
    printf("Num Font: %d\n", fontService_->numFonts());
    printf("Num TextureAtlas: %d\n", textureService_->numTextureAtlases());
    printf("Num Texture: %d\n", textureService_->numTextures());
    printf("Num Texture (sub): %d\n", textureService_->numSubTextures());
    
    printf("Memory %zu\n", textureService_->totalTextureMemory());
    printf("-------- Resource Breakdown END --------\n\n");
}

void BGE::Game::outputMemoryUsage() const {
    printf("\n-------- Memory Usage START --------\n");
    printf("Memory %zu\n", textureService_->totalTextureMemory());
    printf("-------- Memory Usage END --------\n\n");
}
