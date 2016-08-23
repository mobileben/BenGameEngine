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
        ~GameObjectService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}
        
        inline void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }
        inline SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        Space *getSpace(void) const;

        GameObject *createGameObject(std::string name="");
        
        GameObjectHandle getGameObjectHandle(ObjectId objId);
        GameObjectHandle getGameObjectHandle(std::string name);
        GameObject *getGameObject(ObjectId objId);
        GameObject *getGameObject(std::string name);
        GameObject *getGameObject(GameObjectHandle handle);

        void removeGameObject(GameObjectHandle handle);
        
        inline const GameObjectVector& getGameObjects() const { return gameObjects_; }
        
    private:
        friend Space;
        
        using GameObjectHandleService = HandleService<GameObject, GameObjectHandle>;

        static const uint32_t InitialGameObjectReserve = 1024;
        static GameObjectHandleService handleService_;
        
        SpaceHandle         spaceHandle_;
        GameObjectVector    gameObjects_;
    };
}

#endif /* GameObjectService_h */
