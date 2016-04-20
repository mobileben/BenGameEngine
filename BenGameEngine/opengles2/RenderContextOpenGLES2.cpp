//
//  RenderContextOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RenderContextOpenGLES2.h"

BGE::RenderContextOpenGLES2::RenderContextOpenGLES2()
{
    context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
}

