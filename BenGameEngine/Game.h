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
#include <iostream>
#include "Service.h"
#include "AnimationService.h"
#include "GameObjectService.h"
#include "MaterialService.h"
#include "RenderService.h"
#include "TextureService.h"
#include "FontService.h"
#include "HeartbeatService.h"
#include "LogicService.h"
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
        std::shared_ptr<LogicService> getLogicService() { return logicService_; }
        
        // Service functions
        void initialize() override;
        void reset() override;
        void enteringBackground() override;
        void enteringForeground() override;
        void pause() override;
        void resume() override;
        void destroy() override;
        void update(double deltaTime) override;
        
        bool isPaused() const { return paused_; }
        void setPaused(bool paused);
        
        void spaceReset(Space *space);
        
        void outputResourceUsage() const;
        void outputMemoryUsage() const;

        void outputResourceBreakdown(uint32_t numTabs=0) const final;
        void outputMemoryBreakdown(uint32_t numTabs=0) const final;

        static void outputValue(uint32_t numTabs, std::string format, va_list args);
        template <typename... Args> static void outputValue(uint32_t numTabs, std::string format, Args... args) {
            std::string results = format;
            
            for (auto i=0;i<numTabs;i++) {
                printf("\t");
            }
            
            std::vector<AnyToString> vec = {args...};
            
            for (auto &e : vec) {
                auto sVal = e.toString();
                auto specifiers = e.getFormatSpecififers();
                for (auto s : specifiers) {
                    auto f = results.find(s);
                    
                    if (f != std::string::npos) {
                        results.replace(f, s.length(), sVal);
                        break;
                    }
                }
            }
            
            printf("%s", results.c_str());
        }

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
        std::shared_ptr<LogicService>           logicService_;
        std::shared_ptr<EventService>           eventService_;

    private:
        struct AnyToString {
            enum class type {
                SizeT, Int, UInt, Double, String
            };
            
            AnyToString(size_t e) { mData.SIZET = e; mType = type::SizeT; formatSpecifiers = { "%zd", "%d" }; }
            AnyToString(int32_t e) { mData.INT = e; mType = type::Int; formatSpecifiers = { "%d", "%u" }; }
            AnyToString(uint32_t e) { mData.UINT = e; mType = type::UInt; formatSpecifiers = { "%u", "%d" }; }
            AnyToString(float e) { mData.DOUBLE = e; mType = type::Double; formatSpecifiers = { "%f" }; }
            AnyToString(double e) { mData.DOUBLE = e; mType = type::Double; formatSpecifiers = { "%f" }; }
            AnyToString(char *e) { mData.STRING = e; mType = type::String; formatSpecifiers = { "%s" }; }
            AnyToString(const char *e) { mData.STRING = e; mType = type::String; formatSpecifiers = { "%s" }; }

            std::string toString() const;
            std::vector<std::string> getFormatSpecififers() const { return formatSpecifiers; }
            
            private:
                type mType;
                std::vector<std::string> formatSpecifiers;
                union {
                    int32_t     INT;
                    uint32_t    UINT;
                    size_t      SIZET;
                    double      DOUBLE;
                    const char  *STRING;
                } mData;
        };
        
        bool paused_;
        
        // Used to access resources metrics
        std::shared_ptr<ComponentService>       componentService_;
        std::shared_ptr<GameObjectService>      gameObjectService_;
    };
}

#endif /* BGEGame_h */
