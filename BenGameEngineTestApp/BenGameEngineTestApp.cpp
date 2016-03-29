//
//  BenGameEngineTestApp.cpp
//  BenGameEngineTestApp
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BenGameEngineTestApp.hpp"
#include "BGERenderServiceOpenGLES2.h"
#include "BGEGLKView.h"


BenGameEngineTestApp::BenGameEngineTestApp()
{
    BGEGame::getInstance()->provide(std::make_shared<BGERenderServiceOpenGLES2>());
}

void BenGameEngineTestApp::initialize() {
    BGEGame::getInstance()->initialize();
}