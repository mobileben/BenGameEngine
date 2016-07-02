//
//  ScenePackageService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ScenePackageService_h
#define ScenePackageService_h

#include <stdio.h>
#include <functional>
#include <string>
#include <unordered_map>
#include "Handle.h"
#include "HandleService.h"
#include "Error.h"
#include "GraphicFormats.h"
#include "Service.h"
#include "Space.h"
#include "ScenePackage.h"

// TODO: Remove?
#include "TransformComponent.h"
#include "LineRenderComponent.h"
#include "FlatRectRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "AnimationSequenceComponent.h"

namespace BGE {
    class ScenePackageService : public Service {
    public:
        ScenePackageService();
        ~ScenePackageService() {}
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}
        
        void packageFromJSONFile(std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<Error>)> callback);
        
        ScenePackageHandle getPackage(std::string name);
        ScenePackage *getDereferencedPackage(std::string name);
        ScenePackage *getDereferencedPackage(ScenePackageHandle handle);

        void deletePackage(std::string name);
        void deletePackage(ScenePackageHandle handle);
        void resetPackage(std::string name);
        void resetPackage(ScenePackageHandle handle);

        TextureReference *getTextureReference(std::string name);
        TextReference *getTextReference(std::string name);
        AnimationSequenceReference *getAnimationSequenceReference(std::string name);

        std::shared_ptr<SpriteRenderComponent> createSpriteRenderComponent(std::string name);
        std::shared_ptr<TextComponent> createTextComponent(std::string name);
        std::shared_ptr<AnimationSequenceComponent> createAnimationSequenceComponent(std::string name);
        
        void link();
        void link(ScenePackageHandle handle);
        
    private:
        static const uint32_t InitialScenePackageReserve = 32;
        
        typedef HandleService<ScenePackage, ScenePackageHandle> ScenePackageHandleService;
        typedef std::unordered_map<std::string, ScenePackageHandle> ScenePackagesMap;
        typedef std::unordered_map<std::string, ScenePackageHandle>::iterator ScenePackagesMapIterator;
        
        ScenePackageHandleService scenePackageHandleService_;
        ScenePackagesMap scenePackages_;
    };
}

#endif /* ScenePackageService_h */
