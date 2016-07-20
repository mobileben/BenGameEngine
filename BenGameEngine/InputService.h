//
//  InputService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef InputService_h
#define InputService_h

#include <stdio.h>
#include "Service.h"
#include "Input.h"
#include "HandleService.h"

namespace BGE {
    class InputService : public Service {
    public:
        InputService();
        ~InputService();
        
        void initialize();
        void reset();
        void enteringBackground();
        void enteringForeground();
        void pause();
        void resume();
        void destroy();
        void update(double deltaTime) {}
        
        InputHandle createInput();

        void removeInput(InputHandle handle);
        void removeInput(ObjectId inputId);

        InputHandle getInputHandle(ObjectId inputId);
        Input *getInput(InputHandle handle);
        Input *getInput(ObjectId inputId);
        
    private:
        static const uint32_t InitialInputReserve = 64;
        
        using InputHandleService = HandleService<Input, InputHandle>;
        
        using InputMap = std::unordered_map<ObjectId, InputHandle>;
        using InputMapIterator = InputMap::iterator;
        
        InputHandleService   handleService_;
        InputMap             inputs_;
    };
}

#endif /* InputService_h */
