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
#include "GameObjectService.h"
#include <type_traits>

namespace BGE {
    class SpaceService : public Service {
    public:
        SpaceService();
        ~SpaceService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}

        std::shared_ptr<Space> createSpace(std::string name);
        void removeSpace(std::shared_ptr<Space> space);
        void removeSpace(uint64_t objId);
        void removeSpace(std::string name);

        std::shared_ptr<Space> find(std::shared_ptr<Space> space);
        std::shared_ptr<Space> find(uint64_t spaceId);
        std::shared_ptr<Space> find(std::string name);

    private:
        std::unordered_map<uint64_t, std::shared_ptr<Space>> spaces_;
    };
}

#endif /* BGESpaceService_h */
