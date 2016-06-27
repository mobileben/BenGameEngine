//
//  FontServiceOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontServiceOpenGLES2_h
#define BGEFontServiceOpenGLES2_h

#include <stdio.h>
#include "FontService.h"

namespace BGE {
    class FontServiceOpenGLES2 : public FontService
    {
    public:
        FontServiceOpenGLES2(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        
        virtual void initialize() {}
        virtual void reset() {}
        virtual void enteringBackground() {}
        virtual void enteringForeground() {}
        virtual void pause() {}
        virtual void resume() {}
        virtual void destroy() {}
        void update(double deltaTime) {}
        
        void unloadFont(std::string name, uint32_t pixelSize);
    };
}

#endif /* BGEFontServiceOpenGLES2_h */
