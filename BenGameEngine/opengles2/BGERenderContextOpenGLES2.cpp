//
//  BGERenderContextOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGERenderContextOpenGLES2.h"

BGERenderContextOpenGLES2::BGERenderContextOpenGLES2()
{
    context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
}

