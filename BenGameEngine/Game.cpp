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
    gameObjectService_ = std::make_shared<BGE::GameObjectService>();
    componentService_ = std::make_shared<BGE::ComponentService>();
    materialService_ = std::make_shared<BGE::MaterialService>();
    heartbeatService_ = std::make_shared<BGE::HeartbeatService>();
}

void BGE::Game::provide(std::shared_ptr<BGE::RenderService> renderService) {
    renderService_ = renderService;
}

void BGE::Game::provide(std::shared_ptr<TextureService> textureService) {
    textureService_ = textureService;
}

void BGE::Game::provide(std::shared_ptr<FontService> fontService) {
    fontService_ = fontService;
}

void BGE::Game::initialize() {
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
