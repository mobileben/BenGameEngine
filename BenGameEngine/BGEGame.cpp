//
//  BGEGame.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGEGame.h"

std::shared_ptr<BGEGame> BGEGame::getInstance() {
    static std::shared_ptr<BGEGame> instance = std::make_shared<BGEGame>();
    return instance;
}

BGEGame::BGEGame()
{
    heartbeatService_ = std::make_shared<BGEHeartbeatService>();
}

void BGEGame::provide(std::shared_ptr<BGERenderService> renderService) {
    renderService_ = renderService;
}

void BGEGame::provide(std::shared_ptr<BGETextureService> textureService) {
    textureService_ = textureService;
}

void BGEGame::provide(std::shared_ptr<BGEFontService> fontService) {
    fontService_ = fontService;
}

void BGEGame::initialize() {
}

void BGEGame::reset() {
}

void BGEGame::enteringBackground() {
}

void BGEGame::enteringForeground() {
}

void BGEGame::pause() {
}

void BGEGame::resume() {
}

void BGEGame::destroy() {
}
