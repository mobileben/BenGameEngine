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
#include <pthread.h>
#include <atomic>
#include "Object.h"

namespace BGE {
    class Service
    {
    public:
        Service();
        
        virtual ~Service() {}
        
        virtual void initialize() =0;
        virtual void reset() =0;
        virtual void enteringBackground() { backgrounded_ = true; };
        virtual void enteringForeground() { backgrounded_ = false; };
        virtual void pause() { paused_ = true; };
        virtual void resume() { paused_ = false; };
        virtual void destroy() =0;
        virtual void update(double deltaTime) =0;
        
        virtual uint32_t numUsedHandles() const {
            return 0;
        }
        
        virtual uint32_t maxHandles() const {
            return 0;
        }
        
        virtual uint32_t numHandleResizes() const {
            return 0;
        }
        
        virtual uint32_t maxHandlesAllocated() const {
            return 0;
        }
        
        virtual size_t usedHandleMemory() const {
            return 0;
        }
        
        virtual size_t unusedHandleMemory() const {
            return 0;
        }
        
        virtual size_t totalHandleMemory() const {
            return 0;
        }
        
        virtual size_t overheadMemory() const {
            return 0;
        }
        
        virtual void outputResourceBreakdown(uint32_t numTabs) const {};
        virtual void outputMemoryBreakdown(uint32_t numTabs) const {};
        
        bool isBackgrounded() const { return backgrounded_; }
        bool isPaused() const { return paused_; }
        
        // Synchronization
        void lock();
        void unlock();

    protected:
        ObjectId getIdAndIncrement();
        
    private:
        pthread_mutex_t mutex_;
        
        std::atomic<ObjectId>   identifier_;
        
        bool                    backgrounded_;
        bool                    paused_;
    };
}

#endif /* BGEService_h */
