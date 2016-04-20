//
//  RenderContextOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderContextOpenGLES2_h
#define BGERenderContextOpenGLES2_h
#include <GLKit/GLKit.h>

#include <stdio.h>
#include "RenderContext.h"

namespace BGE {
    class RenderContextOpenGLES2 : public RenderContext
    {
    public:
        RenderContextOpenGLES2();
        
        EAGLContext *getContext() { return context_; }
        
    private:
        EAGLContext *context_;
    };
}

#endif /* BGERenderContextOpenGLES2_h */
