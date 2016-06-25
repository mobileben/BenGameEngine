//
//  Service.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEService_h
#define BGEService_h

#include <stdio.h>
#include <atomic>

namespace BGE {
    class Service
    {
    public:
        Service() : identifier_(1) {}
        virtual ~Service() {}
        
        virtual void initialize() =0;
        virtual void reset() =0;
        virtual void enteringBackground() =0;
        virtual void enteringForeground() =0;
        virtual void pause() =0;
        virtual void resume() =0;
        virtual void destroy() =0;
        virtual void update(double deltaTime) =0;
        
    protected:
        uint64_t getIdAndIncrement();
        
    private:
        std::atomic<uint64_t>   identifier_;
    };
}

#endif /* BGEService_h */
