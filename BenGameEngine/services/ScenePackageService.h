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
#include "FileUtilities.h"
#include <thread>

#include "rapidjson/document.h"

namespace BGE {
    using ScenePackageLoadCompletionHandler = std::function<void(ScenePackageHandle, std::shared_ptr<Error>)>;

    struct ScenePackageLoadItem {
        enum class LoadType { Package, Textures, Function };
        LoadType            type;
        SpaceHandle         spaceHandle;
        ScenePackageHandle  packageHandle;
        std::string         name;
        FilePath            filePath;
        ScenePackageLoadCompletionHandler completionHandler;

        ScenePackageLoadItem() : type(LoadType::Package), completionHandler(nullptr) {}
        ScenePackageLoadItem(SpaceHandle spaceHandle, const std::string& name, const FilePath& filePath, ScenePackageLoadCompletionHandler callback) : type(LoadType::Package), spaceHandle(spaceHandle), name(name), filePath(filePath), completionHandler(callback) {}
        ScenePackageLoadItem(ScenePackageHandle packageHandle, ScenePackageLoadCompletionHandler callback) : type(LoadType::Textures), packageHandle(packageHandle), completionHandler(callback) {}
        // Can be used to execute a function on the scene package thread.
        ScenePackageLoadItem(ScenePackageLoadCompletionHandler callback) : type(LoadType::Function), completionHandler(callback) {}
    };
    
    class ScenePackageService : public Service {
    public:
        ScenePackageService();
        ~ScenePackageService();
        
        void initialize() final;
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(__attribute__ ((unused)) double deltaTime) final {}
        void garbageCollect() final { handleService_.garbageCollect(); }
        
        uint32_t numUsedHandles() const final;
        uint32_t maxHandles() const final;
        uint32_t numHandleResizes() const final;
        uint32_t maxHandlesAllocated() const final;

        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;
        size_t totalMemory() const;

        uint32_t numScenePackages() const;

        bool isAligned8Memory(size_t size) const;
        size_t aligned8MemorySize(size_t size) const;

        void dispatchAsync(ScenePackageLoadCompletionHandler func);

        void createPackage(SpaceHandle spaceHandle, std::string name, const FilePath &filePath, ScenePackageLoadCompletionHandler callback);
        void createPackageFromSPKGBinary(SpaceHandle spaceHandle, std::string name, const uint64_t *buffer, size_t bufferSize, bool managed, const BaseDirectory& baseDirectory, ScenePackageLoadCompletionHandler callback);
        void createPackageFromJSONBinary(SpaceHandle spaceHandle, std::string name, const uint8_t *buffer, size_t bufferSize, const BaseDirectory &baseDirectory, ScenePackageLoadCompletionHandler callback);
        void createPackageFromJSONDict(SpaceHandle spaceHandle, std::string name, const std::shared_ptr<rapidjson::Document> jsonDict, const BaseDirectory &baseDirectory, ScenePackageLoadCompletionHandler callback);
        
        void addSpaceHandleReference(SpaceHandle spaceHandle, ScenePackageHandle packageHandle);

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
        TextureMaskReference *getTextureMaskReference(std::string name) const;
        PlacementReference *getPlacementReference(std::string name) const;
        TextureReference *getTextureReference(std::string name) const;
        TextReference *getTextReference(std::string name) const;
        ExternalPackageReference *getExternalReference(std::string name) const;
        
        // TODO: Remove this as this is a hack
        GfxReferenceType getReferenceType(std::string name) const;
        
        void link();
        void link(ScenePackageHandle handle);
        
        void outputResourceBreakdown(uint32_t numTabs) const final;
        void outputMemoryBreakdown(uint32_t numTabs) const final;

    private:
        static const uint32_t InitialScenePackageReserve = 32;
        
        using ScenePackageHandleService = HandleService<ScenePackage, ScenePackageHandle>;
        
        struct ScenePackageReference {
            ScenePackageHandle          handle;
            std::vector<SpaceHandle>    references;
        };
        
        bool                                threadRunning_;
        std::mutex                          threadRunningMutex_;
        std::thread                         thread_;
        ScenePackageHandleService           handleService_;
        std::vector<ScenePackageReference>  scenePackages_;
        Queue<ScenePackageLoadItem>         queuedLoadItems_;
        
        void loadThreadFunction();
        void loadPackageTextures(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void createPackage(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void createPackageFromJSON(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void createPackageFromSPKG(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void completionPackage(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback);
        void releasePackage(ScenePackage *package);
    };
}

#endif /* ScenePackageService_h */
