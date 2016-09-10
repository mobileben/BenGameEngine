//
//  BenGameEngineTestApp.cpp
//  BenGameEngineTestApp
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BenGameEngineTestApp.hpp"
#include "RenderServiceOpenGLES2.h"
#include "FileUtilities.h"

BenGameEngineTestApp::BenGameEngineTestApp()
{
    BGE::Game::getInstance()->provide(std::make_shared<BGE::RenderServiceOpenGLES2>());
}

void BenGameEngineTestApp::initialize() {
    BGE::Game::getInstance()->initialize();
    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsPath = [searchPaths objectAtIndex:0];
    
    BGE::FileUtilities::initialize([[NSBundle mainBundle].bundlePath UTF8String], [documentsPath UTF8String]);
}