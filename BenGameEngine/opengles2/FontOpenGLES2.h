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
        FontOpenGLES2(std::string name, uint32_t pixelSize);
        ~FontOpenGLES2() {}
        
        void load(std::string filename, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback);
        void drawString(std::string str, Vector2 &position, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
    };
}

#endif /* BGEFontOpenGLES2_h */
