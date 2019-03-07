//
//  GameObjectService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef GameObjectService_h
#define GameObjectService_h

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Service.h"
#include "GameObject.h"
#include "HandleService.h"
#include <type_traits>

namespace BGE {
    class Space;
    
    class GameObjectService : public Service {
    public:
        using GameObjectVector = std::vector<GameObjectHandle>;
        
        GameObjectService();
        ~GameObjectService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(__attribute__ ((unused)) double deltaTime) final {}
        void garbageCollect() final { handleService_.garbageCollect(); }
        
        uint32_t numGameObjects () const;
        
        uint32_t numUsedHandles() const final;
        uint32_t maxHandles() const final;
        uint32_t numHandleResizes() const final;
        uint32_t maxHandlesAllocated() const final;

        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;
        
        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        Space *getSpace(void) const;

        GameObject *createGameObject(std::string name="");
        
        GameObjectHandle getGameObjectHandle(ObjectId objId) const;
        GameObjectHandle getGameObjectHandleLockless(ObjectId objId) const;
        GameObjectHandle getGameObjectHandle(std::string name) const;
        GameObjectHandle getGameObjectHandleLockless(std::string name) const;

        inline GameObject *getGameObject(ObjectId objId) const {
            return handleService_.dereference(getGameObjectHandle(objId));
        }
        inline GameObject *getGameObjectLockless(ObjectId objId) const {
            return handleService_.dereferenceLockless(getGameObjectHandleLockless(objId));
        }

        inline GameObject *getGameObject(std::string name) const  {
            return handleService_.dereference(getGameObjectHandle(name));
        }
        inline GameObject *getGameObjectLockless(std::string name) const  {
            return handleService_.dereferenceLockless(getGameObjectHandleLockless(name));
        }

        inline GameObject *getGameObject(GameObjectHandle handle) const  {
            return handleService_.dereference(handle);
        }
        inline GameObject *getGameObjectLockless(GameObjectHandle handle) const  {
            return handleService_.dereferenceLockless(handle);
        }

#ifdef NOT_YET
        void removeGameObject(GameObjectHandle handle);
        
        inline void removeGameObject(GameObject *object) {
            if (object) {
                removeGameObject(object->getHandle());
            }
        }
#else
        inline void removeGameObject(GameObjectHandle handle) {
            auto object = getGameObject(handle);
            
            if (object) {
                removeGameObject(object);
            }
        }
        
        void removeGameObject(GameObject *object);
#endif
        
        void removeAllGameObjects();
        
        inline const GameObjectVector& getGameObjects() const { return gameObjects_; }
        
    private:
        friend Space;
        
        using GameObjectHandleService = HandleService<GameObject, GameObjectHandle>;

        static const uint32_t InitialGameObjectReserve = 32768;
        static GameObjectHandleService handleService_;
        
        SpaceHandle         spaceHandle_;
        GameObjectVector    gameObjects_;
        
        std::vector<GameObject*>    childGameObjectsScratch_;
        
        void getAllChildGameObjects(GameObject *root, std::vector<GameObject *> &objects);
        void releaseObject(GameObjectHandle handle);
        void releaseObjectDontReleaseComponents(GameObjectHandle handle);
    };
}

#endif /* GameObjectService_h */
