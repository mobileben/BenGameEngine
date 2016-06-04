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
#include "ScenePackage.h"

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
        
        void packageFromJSONFile(std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<BGE::Error>)> callback);
        
        void getPackage(std::string name);
        void deletePackage(std::string name);
        void resetPackage(std::string name);
        
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
