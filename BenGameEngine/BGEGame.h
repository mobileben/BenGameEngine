//
//  BGEGame.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEGame_h
#define BGEGame_h

#include <stdio.h>
#include <memory>
#include "BGEService.h"
#include "BGERenderService.h"
#include "BGETextureService.h"
#include "BGEFontService.h"
#include "BGEHeartbeatService.h"

class BGEGame : public BGEService
{
public:
    BGEGame();
    virtual ~BGEGame() {}
    
    static std::shared_ptr<BGEGame> getInstance();
    std::shared_ptr<BGERenderService> getRenderService() { return renderService_; }
    void provide(std::shared_ptr<BGERenderService> renderService);

    std::shared_ptr<BGETextureService> getTextureService() { return textureService_; }
    void provide(std::shared_ptr<BGETextureService> textureService);
    
    std::shared_ptr<BGEFontService> getFontService() { return fontService_; }
    void provide(std::shared_ptr<BGEFontService> fontService);
    
    std::shared_ptr<BGEHeartbeatService> getHeartbeatService() { return heartbeatService_; }
    // No provide for heartbeat as it is implicitly built
    
    // BGEService functions
    void initialize();
    void reset();
    void enteringBackground();
    void enteringForeground();
    void pause();
    void resume();
    void destroy();

protected:
    std::shared_ptr<BGERenderService> renderService_;
    std::shared_ptr<BGETextureService> textureService_;
    std::shared_ptr<BGEFontService> fontService_;
    std::shared_ptr<BGEHeartbeatService> heartbeatService_;
};

#endif /* BGEGame_h */
