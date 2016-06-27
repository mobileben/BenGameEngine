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
    class TransformComponent;
    
    class FontOpenGLES2 : public Font {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<FontOpenGLES2> create(std::string name, uint32_t pixelSize);

        FontOpenGLES2(struct private_key const& key, std::string name, uint32_t pixelSize);
        ~FontOpenGLES2() {}
        
        void load(std::string filename, uint32_t faceIndex, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback);
        
        // TODO: Determine if font rendering should be done else where, like in the renderer versus the font.
        // TODO: We will want to cache the width/height of the string. So this is probably not the best place for the drawString to exist
        void drawString(std::string str, std::shared_ptr<TransformComponent> transform, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
        void drawString(std::string str, Vector2 &position, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
        
    protected:
        void drawString(std::string str, const float *rawMatrix, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
    };
}

#endif /* BGEFontOpenGLES2_h */
