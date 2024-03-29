//
//  SpaceService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpaceService_h
#define BGESpaceService_h

#include <stdio.h>
#include <memory>
#include <unordered_map>
#include "Service.h"
#include "Space.h"
#include "HandleService.h"
#include "Queue.h"

#include <type_traits>

namespace BGE {
    class SpaceService : public Service, public std::enable_shared_from_this<SpaceService> {
    public:
        SpaceService();
        ~SpaceService();
        
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
        
        uint32_t numSpaces() const;
        
        void outputResourceBreakdown(uint32_t numTabs) const final;
        void outputMemoryBreakdown(uint32_t numTabs) const final;

        Space *createSpace(std::string name, uint32_t order=0);

        void removeSpace(SpaceHandle spaceHandle);
        void removeSpace(std::string name);

        void queueReset(SpaceHandle spaceHandle);

        Space *getSpace(SpaceHandle spaceHandle) const;
        Space *getSpaceLockless(SpaceHandle spaceHandle) const;
        Space *getSpace(std::string name) const;
        Space *getSpaceLockless(std::string name) const;

        std::vector<SpaceHandle> getSpaces() const;
        void getSpaces(std::vector<SpaceHandle>& spaces) const;
        std::vector<SpaceHandle> getReversedSpaces() const;
        void getReversedSpaces(std::vector<SpaceHandle>& spaces) const;
        
        void reorderSpaces();
        
    private:
        static const uint32_t InitialSpaceReserve = 32;
        
        using SpaceHandleService = HandleService<Space, SpaceHandle>;
        using SpacesMap = std::unordered_map<uint32_t, SpaceHandle>;
        using SpacesMapIterator = SpacesMap::iterator;

        SpaceHandleService          handleService_;
        std::vector<SpaceHandle>    spaces_;
        Queue<SpaceHandle>          resetQueue_;

        // TODO: Should just create sorted vector here each time space is removed or added, but also must think how to handle order change

        bool        threadRunning_;
        std::mutex  threadRunningMutex_;
        std::thread thread_;

        void threadFunction();

    };
}

#endif /* BGESpaceService_h */
