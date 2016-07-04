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
        
        ScenePackageHandle getScenePackageHandle(ObjectId scenePackageId);
        ScenePackageHandle getScenePackageHandle(std::string name);
        
        ScenePackage *getScenePackage(ObjectId scenePackageId);
        ScenePackage *getScenePackage(std::string name);
        ScenePackage *getScenePackage(ScenePackageHandle handle);

        void removePackage(ObjectId scenePackageId);
        void removePackage(std::string name);
        void removePackage(ScenePackageHandle handle);
        
        void resetPackage(std::string name);
        void resetPackage(ScenePackageHandle handle);

        TextureReference *getTextureReference(std::string name);
        TextReference *getTextReference(std::string name);
        AnimationSequenceReference *getAnimationSequenceReference(std::string name);
        
        void link();
        void link(ScenePackageHandle handle);
        
    private:
        static const uint32_t InitialScenePackageReserve = 32;
        
        using ScenePackageHandleService = HandleService<ScenePackage, ScenePackageHandle>;
        using ScenePackagesMap = std::unordered_map<ObjectId, ScenePackageHandle>;
        using ScenePackagesMapIterator = ScenePackagesMap::iterator;
        
        ScenePackageHandleService   handleService_;
        ScenePackagesMap            scenePackages_;
    };
}

#endif /* ScenePackageService_h */
