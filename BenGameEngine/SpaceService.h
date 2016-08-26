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
#include <type_traits>

namespace BGE {
    class SpaceService : public Service {
    public:
        SpaceService();
        ~SpaceService() {}
        
        void initialize() final {}
        void reset() final {}
        void enteringBackground() final {}
        void enteringForeground() final {}
        void pause() final {}
        void resume() final {}
        void destroy() final {}
        void update(double deltaTime) final {}

        SpaceHandle createSpace(std::string name);
        
        void removeSpace(SpaceHandle spaceHandle);
        void removeSpace(ObjectId objId);
        void removeSpace(std::string name);

        Space *getSpace(SpaceHandle spaceHandle) const;
        Space *getSpace(ObjectId objId) const;
        Space *getSpace(std::string name) const;
        
        std::vector<SpaceHandle> getSpaces() const;
        
    private:
        static const uint32_t InitialSpaceReserve = 32;
        
        using SpaceHandleService = HandleService<Space, SpaceHandle>;
        using SpacesMap = std::unordered_map<uint32_t, SpaceHandle>;
        using SpacesMapIterator = SpacesMap::iterator;

        SpaceHandleService  handleService_;
        SpacesMap           spaces_;
        
        // TODO: Should just create sorted vector here each time space is removed or added, but also must think how to handle order change
    };
}

#endif /* BGESpaceService_h */
