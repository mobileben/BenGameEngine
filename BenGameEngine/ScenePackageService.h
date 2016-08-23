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
#include "ButtonComponent.h"
#include "MaskComponent.h"

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
        
        void packageFromJSONFile(SpaceHandle spaceHandle, std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<Error>)> callback);
        
        ScenePackageHandle getScenePackageHandle(ObjectId scenePackageId);
        ScenePackageHandle getScenePackageHandle(std::string name);
        
        ScenePackage *getScenePackage(ObjectId scenePackageId);
        ScenePackage *getScenePackage(std::string name);
        ScenePackage *getScenePackage(ScenePackageHandle handle);

        void removePackage(SpaceHandle spaceHandle, ObjectId scenePackageId);
        void removePackage(SpaceHandle spaceHandle, std::string name);
        void removePackage(SpaceHandle spaceHandle, ScenePackageHandle handle);
        
        void resetPackage(std::string name);
        void resetPackage(ScenePackageHandle handle);

        AnimationSequenceReference *getAnimationSequenceReference(std::string name);
        ButtonReference *getButtonReference(std::string name);
        MaskReference *getMaskReference(std::string name);
        PlacementReference *getPlacementReference(std::string name);
        TextureReference *getTextureReference(std::string name);
        TextReference *getTextReference(std::string name);
        ExternalPackageReference *getExternalReference(std::string name);
        
        // TODO: Remove this as this is a hack
        GfxReferenceType getReferenceType(std::string name);
        
        void link();
        void link(ScenePackageHandle handle);
        
    private:
        static const uint32_t InitialScenePackageReserve = 32;
        
        using ScenePackageHandleService = HandleService<ScenePackage, ScenePackageHandle>;
        
        struct ScenePackageReference {
            ScenePackageHandle          handle;
            std::vector<SpaceHandle>    references;
        };
        
        ScenePackageHandleService           handleService_;
        std::vector<ScenePackageReference>  scenePackages_;
        
        void releasePackage(ScenePackage *package);
    };
}

#endif /* ScenePackageService_h */
