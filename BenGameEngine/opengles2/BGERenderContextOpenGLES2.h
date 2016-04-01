//
//  BGERenderContextOpenGLES2.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderContextOpenGLES2_h
#define BGERenderContextOpenGLES2_h
#include <GLKit/GLKit.h>

#include <stdio.h>
#include "BGERenderContext.h"

class BGERenderContextOpenGLES2 : public BGERenderContext
{
public:
    BGERenderContextOpenGLES2();
    
    EAGLContext *getContext() { return context_; }
    
private:
    EAGLContext *context_;
};

#endif /* BGERenderContextOpenGLES2_h */
