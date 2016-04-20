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
#include <Foundation/Foundation.h>

namespace BGE {
    class FontServiceOpenGLES2 : public FontService
    {
    public:
        static void mapBundles(std::string bundleName);
        
        FontServiceOpenGLES2(std::map<std::string, std::string> resources = std::map<std::string, std::string>());
        
        virtual void initialize() {}
        virtual void reset() {}
        virtual void enteringBackground() {}
        virtual void enteringForeground() {}
        virtual void pause() {}
        virtual void resume() {}
        virtual void destroy() {}
        
        std::shared_ptr<Font> getFont(std::string name, uint32_t pixelSize);
        
        void loadFont(std::string name, uint32_t pixelSize, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback);
        void unloadFont(std::string name, uint32_t pixelSize);
        
    private:
        static NSBundle *builtinBundle_;
        static NSBundle *mainBundle_;
    };
}

#endif /* BGEFontServiceOpenGLES2_h */
