//
//  LogicService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/24/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef LogicService_h
#define LogicService_h

#include <stdio.h>
#include <vector>

#include "Handle.h"
#include "Service.h"

namespace BGE {
    class GameObject;
    class Space;
    
    class LogicService : public Service {
    public:
        LogicService();
        ~LogicService() {}
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final { Service::enteringBackground(); }
        void enteringForeground() final { Service::enteringForeground(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final;
        
        void addGameObject(GameObject *gameObject);
        void removeGameObject(GameObject *gameObject);
        void spaceReset(Space *space);
        
    private:
        std::vector<std::pair<SpaceHandle, GameObjectHandle>> gameObjectHandles_;
    };
}

#endif /* LogicService_h */
