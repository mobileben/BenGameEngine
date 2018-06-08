//
//  RenderContextOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifdef SUPPORT_OPENGLES2

#ifndef BGERenderContextOpenGLES2_h
#define BGERenderContextOpenGLES2_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE

#include <GLKit/GLKit.h>

#endif /* TARGET_OS_IPHONE */

#include <stdio.h>
#include "RenderContext.h"

namespace BGE {
    class RenderContextOpenGLES2 : public RenderContext
    {
    public:
        RenderContextOpenGLES2();

#if TARGET_OS_IPHONE
        EAGLContext *getContext() { return context_; }
#endif /* TARGET_OS_IPHONE */
        
    private:
#if TARGET_OS_IPHONE
        EAGLContext *context_;
#endif /* TARGET_OS_IPHONE */
    };
}

#endif /* BGERenderContextOpenGLES2_h */

#endif /* SUPPORT_OPENGLES2 */
