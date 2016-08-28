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
#include "EventService.h"

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
        std::shared_ptr<EventService> getEventService() { return eventService_ ; }
        
        // Service functions
        void initialize() override;
        void reset() override;
        void enteringBackground() override;
        void enteringForeground() override;
        void pause() override;
        void resume() override;
        void destroy() override;
        void update(double deltaTime) override;
        
        void outputResourceUsage() const;
        void outputMemoryUsage() const;

        void outputResourceBreakdown(uint32_t numTabs=0) const final;

    protected:
        std::shared_ptr<RenderService>          renderService_;
        std::shared_ptr<TextureService>         textureService_;
        std::shared_ptr<FontService>            fontService_;
        std::shared_ptr<HeartbeatService>       heartbeatService_;
        std::shared_ptr<MaterialService>        materialService_;
        std::shared_ptr<ScenePackageService>    scenePackageService_;
        std::shared_ptr<AnimationService>       animationService_;
        std::shared_ptr<SpaceService>           spaceService_;
        std::shared_ptr<InputService>           inputService_;
        std::shared_ptr<EventService>           eventService_;
        
    private:
        // Used to access resources metrics
        std::shared_ptr<ComponentService>       componentService_;
        std::shared_ptr<GameObjectService>      gameObjectService_;
    };
}

#endif /* BGEGame_h */
