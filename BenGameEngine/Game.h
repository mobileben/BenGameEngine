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
#include "AnimationService.h"
#include "GameObjectService.h"
#include "MaterialService.h"
#include "RenderService.h"
#include "TextureService.h"
#include "FontService.h"
#include "HeartbeatService.h"
#include "ComponentService.h"
#include "ScenePackageService.h"
#include "SpaceService.h"
#include "InputService.h"

namespace BGE {
    class Game : public Service
    {
    public:
        Game();
        virtual ~Game() {}
        
        static std::shared_ptr<Game> getInstance();
        std::shared_ptr<RenderService> getRenderService() { return renderService_; }
        void provide(std::shared_ptr<RenderService> renderService);
        
        std::shared_ptr<TextureService> getTextureService() { return textureService_; }
        void provide(std::shared_ptr<TextureService> textureService);
        
        // Services which are implicitly built
        std::shared_ptr<MaterialService> getMaterialService() { return materialService_; }
        std::shared_ptr<HeartbeatService> getHeartbeatService() { return heartbeatService_; }
        std::shared_ptr<ScenePackageService> getScenePackageService() { return scenePackageService_; }
        std::shared_ptr<AnimationService> getAnimationService() { return animationService_; }
        std::shared_ptr<SpaceService> getSpaceService() { return spaceService_; }
        std::shared_ptr<FontService> getFontService() { return fontService_; }
        std::shared_ptr<InputService> getInputService() { return inputService_; }
        
        // Service functions
        void initialize();
        void reset();
        void enteringBackground();
        void enteringForeground();
        void pause();
        void resume();
        void destroy();
        void update(double deltaTime);
        
    protected:
        std::shared_ptr<RenderService> renderService_;
        std::shared_ptr<TextureService> textureService_;
        std::shared_ptr<FontService> fontService_;
        std::shared_ptr<HeartbeatService> heartbeatService_;
        std::shared_ptr<MaterialService> materialService_;
        std::shared_ptr<ScenePackageService> scenePackageService_;
        std::shared_ptr<AnimationService> animationService_;
        std::shared_ptr<SpaceService> spaceService_;
        std::shared_ptr<InputService> inputService_;
    };
}

#endif /* BGEGame_h */
