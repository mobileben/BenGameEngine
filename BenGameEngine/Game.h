//
//  Game.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEGame_h
#define BGEGame_h

#include <stdio.h>
#include <memory>
#include "Service.h"
#include "GameObjectService.h"
#include "MaterialService.h"
#include "RenderService.h"
#include "BGETextureService.h"
#include "BGEFontService.h"
#include "HeartbeatService.h"
#include "ComponentService.h"

namespace BGE {
    class Game : public BGE::Service
    {
    public:
        Game();
        virtual ~Game() {}
        
        static std::shared_ptr<Game> getInstance();
        std::shared_ptr<BGE::RenderService> getRenderService() { return renderService_; }
        void provide(std::shared_ptr<BGE::RenderService> renderService);
        
        std::shared_ptr<BGETextureService> getTextureService() { return textureService_; }
        void provide(std::shared_ptr<BGETextureService> textureService);
        
        std::shared_ptr<BGEFontService> getFontService() { return fontService_; }
        void provide(std::shared_ptr<BGEFontService> fontService);
        
        // Services which are implicitly built
        std::shared_ptr<BGE::GameObjectService> getGameObjectService() { return gameObjectService_; }
        std::shared_ptr<BGE::ComponentService> getComponentService() { return componentService_; }
        std::shared_ptr<BGE::MaterialService> getMaterialService() { return materialService_; }
        std::shared_ptr<BGE::HeartbeatService> getHeartbeatService() { return heartbeatService_; }
        
        // Service functions
        void initialize();
        void reset();
        void enteringBackground();
        void enteringForeground();
        void pause();
        void resume();
        void destroy();
        
    protected:
        std::shared_ptr<BGE::RenderService> renderService_;
        std::shared_ptr<BGETextureService> textureService_;
        std::shared_ptr<BGEFontService> fontService_;
        std::shared_ptr<BGE::HeartbeatService> heartbeatService_;
        std::shared_ptr<BGE::MaterialService> materialService_;
        std::shared_ptr<BGE::GameObjectService> gameObjectService_;
        std::shared_ptr<BGE::ComponentService> componentService_;
    };
}

#endif /* BGEGame_h */
