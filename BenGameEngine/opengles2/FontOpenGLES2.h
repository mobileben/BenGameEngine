//
//  FontOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontOpenGLES2_h
#define BGEFontOpenGLES2_h

#include <stdio.h>
#include "Font.h"

namespace BGE {
    class FontOpenGLES2 : public Font {
    public:
        FontOpenGLES2(std::string name, uint32_t pixelSize, std::string filename);
        ~FontOpenGLES2() {}
        
        void drawString(std::string str, BGEVector2 &position, BGEVector4 &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
    };
}

#endif /* BGEFontOpenGLES2_h */
