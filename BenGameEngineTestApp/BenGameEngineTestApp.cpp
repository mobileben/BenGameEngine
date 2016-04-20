//
//  BenGameEngineTestApp.cpp
//  BenGameEngineTestApp
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BenGameEngineTestApp.hpp"
#include "RenderServiceOpenGLES2.h"
#include "BGEGLKView.h"


BenGameEngineTestApp::BenGameEngineTestApp()
{
    BGEGame::getInstance()->provide(std::make_shared<BGE::RenderServiceOpenGLES2>());
}

void BenGameEngineTestApp::initialize() {
    BGEGame::getInstance()->initialize();
}