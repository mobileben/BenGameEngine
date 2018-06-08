//
//  RenderContextOpenGLES2.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#include "RenderContextOpenGLES2.h"

BGE::RenderContextOpenGLES2::RenderContextOpenGLES2()
{
#if TARGET_OS_IPHONE
    context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#endif /* TARGET_OS_IPHONE */
}

#endif /* SUPPORT_OPENGLES2 */

