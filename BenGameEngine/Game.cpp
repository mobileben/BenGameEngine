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
