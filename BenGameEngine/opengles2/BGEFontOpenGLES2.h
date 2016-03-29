//
//  BGEFontOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGEFontOpenGLES2_h
#define BGEFontOpenGLES2_h

#include <stdio.h>
#include "BGEFont.h"

class BGEFontOpenGLES2 : public BGEFont {
public:
    BGEFontOpenGLES2(std::string name, uint32_t pixelSize, std::string filename);
    ~BGEFontOpenGLES2() {}
    
    void drawString(std::string str, BGEVector2 &position, BGEVector4 &color, BGEFontHorizontalAlignment horizAlignment=BGEFontHorizontalAlignment::Center, BGEFontVerticalAlignment vertAlignment=BGEFontVerticalAlignment::Center, bool minimum=true);
};

#endif /* BGEFontOpenGLES2_h */
