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
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}
        
        uint32_t numGameObjects () const;
        
        size_t usedHandleMemory() const final;
        size_t unusedHandleMemory() const final;
        size_t totalHandleMemory() const final;
        
        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        Space *getSpace(void) const;

        GameObject *createGameObject(std::string name="");
        
        GameObjectHandle getGameObjectHandle(ObjectId objId) const;
        GameObjectHandle getGameObjectHandle(std::string name) const;
        inline GameObject *getGameObject(ObjectId objId) const {
            return handleService_.dereference(getGameObjectHandle(objId));
        }
        
        inline GameObject *getGameObject(std::string name) const  {
            return handleService_.dereference(getGameObjectHandle(name));
        }
        
        inline GameObject *getGameObject(GameObjectHandle handle) const  {
            return handleService_.dereference(handle);
        }
        
        void removeGameObject(GameObjectHandle handle);
        
        inline void removeGameObject(GameObject *object) {
            if (object) {
                removeGameObject(object->getHandle());
            }
        }
        
        void removeAllGameObjects();
        
        inline const GameObjectVector& getGameObjects() const { return gameObjects_; }
        
    private:
        friend Space;
        
        using GameObjectHandleService = HandleService<GameObject, GameObjectHandle>;

        static const uint32_t InitialGameObjectReserve = 1024;
        static GameObjectHandleService handleService_;
        
        SpaceHandle         spaceHandle_;
        GameObjectVector    gameObjects_;
        
        void releaseObject(GameObjectHandle handle);
    };
}

#endif /* GameObjectService_h */
