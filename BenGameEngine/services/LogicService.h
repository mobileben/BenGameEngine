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
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final;
        
        void addGameObject(GameObject *gameObject);
        void removeGameObject(GameObject *gameObject);
        void spaceReset(Space *space);

#ifdef SUPPORT_PROFILING
        int32_t getNumProcessedObjects() const { return numProcessedObjects_; }
        int64_t getProcessingTime() const { return processingTime_; }
#endif /* SUPPORT_PROFILING */
        
    private:
        enum class Operation : uint32_t {
            None = 0,
            Add,
            Remove
        };
        
        struct LogicElement {
            Operation           op;
            SpaceHandle         spaceHandle;
            GameObjectHandle    gameObjHandle;
        };
        
        pthread_mutex_t                                         addRemoveMutex_;
        pthread_mutex_t                                         executeMutex_;
        std::vector<std::pair<SpaceHandle, GameObjectHandle>>   gameObjectHandles_;
        std::vector<LogicElement>                               addRemoveQueue_;

#ifdef SUPPORT_PROFILING
        int32_t     numProcessedObjects_;
        int64_t     processingTime_;
#endif /* SUPPORT_PROFILING */

        void addRemoveLock();
        void addRemoveUnlock();
        void executeLock();
        void executeUnlock();
    };
}

#endif /* LogicService_h */
