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
#include "Queue.h"
#include <thread>

namespace BGE {
    using ScenePackageLoadCompletionHandler = std::function<void(ScenePackageHandle, std::shared_ptr<Error>)>;
    
    struct ScenePackageLoadItem {
        SpaceHandle spaceHandle;
        std::string name;
        std::string filename;
        ScenePackageLoadCompletionHandler completionHandler;
    };
    
    class ScenePackageService : public Service {
    public:
        ScenePackageService();
        ~ScenePackageService() {}
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}
        
        uint32_t numScenePackages() const;
        
        void createPackage(SpaceHandle spaceHandle, std::string name, std::string filename, ScenePackageLoadCompletionHandler callback);
        
        ScenePackageHandle getScenePackageHandle(std::string name) const;
        
        ScenePackage *getScenePackage(std::string name) const;
        ScenePackage *getScenePackage(ScenePackageHandle handle) const;

        void removePackage(SpaceHandle spaceHandle, std::string name);
        void removePackage(SpaceHandle spaceHandle, ScenePackageHandle handle);
        
        void resetPackage(std::string name);
        void resetPackage(ScenePackageHandle handle);

        AnimationSequenceReference *getAnimationSequenceReference(std::string name) const;
        ButtonReference *getButtonReference(std::string name) const;
        MaskReference *getMaskReference(std::string name) const;
        PlacementReference *getPlacementReference(std::string name) const;
        TextureReference *getTextureReference(std::string name) const;
        TextReference *getTextReference(std::string name) const;
        ExternalPackageReference *getExternalReference(std::string name) const;
        
        // TODO: Remove this as this is a hack
        GfxReferenceType getReferenceType(std::string name) const;
        
        void link();
        void link(ScenePackageHandle handle);
        
    private:
        static const uint32_t InitialScenePackageReserve = 32;
        
        using ScenePackageHandleService = HandleService<ScenePackage, ScenePackageHandle>;
        
        struct ScenePackageReference {
            ScenePackageHandle          handle;
            std::vector<SpaceHandle>    references;
        };
        
        std::thread                         loadThread_;
        ScenePackageHandleService           handleService_;
        std::vector<ScenePackageReference>  scenePackages_;
        Queue<ScenePackageLoadItem>         queuedLoadItems_;
        
        void loadThreadFunction();
        void createPackage(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void createPackageFromJSON(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void createPackageFromSPKG(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void releasePackage(ScenePackage *package);
    };
}

#endif /* ScenePackageService_h */
