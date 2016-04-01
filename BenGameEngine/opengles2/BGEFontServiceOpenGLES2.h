//
//  BGEFontServiceOpenGLES2.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/15/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFontServiceOpenGLES2_h
#define BGEFontServiceOpenGLES2_h

#include <stdio.h>
#include "BGEFontService.h"
#include <Foundation/Foundation.h>

class BGEFontServiceOpenGLES2 : public BGEFontService
{
public:
    static void mapBundles(std::string bundleName);
    
    BGEFontServiceOpenGLES2(std::map<std::string, std::string> resources = std::map<std::string, std::string>());

    virtual void initialize() {}
    virtual void reset() {}
    virtual void enteringBackground() {}
    virtual void enteringForeground() {}
    virtual void pause() {}
    virtual void resume() {}
    virtual void destroy() {}

    std::shared_ptr<BGEFont> getFont(std::string name, uint32_t pixelSize);
    
    void loadFont(std::string name, uint32_t pixelSize, std::function<void(std::shared_ptr<BGEFont>, std::shared_ptr<BGEError> error)> callback);
    void unloadFont(std::string name, uint32_t pixelSize);
    
private:
    static NSBundle *builtinBundle_;
    static NSBundle *mainBundle_;
};

#endif /* BGEFontServiceOpenGLES2_h */
