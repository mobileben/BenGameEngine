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
#include <functional>
#include "Service.h"
#include "Input.h"
#include "HandleService.h"

namespace BGE {
    struct {
        std::function<void(Input *)> listener;
        void *object;
    } InputListener;
    
    class InputService : public Service {
    public:
        InputService();
        ~InputService();
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}
        
        void touchEventDown(NSSet* touches, UIView* view);
        void touchEventUp(NSSet* touches, UIView* view);
        void touchEventMove(NSSet* touches, UIView* view);
        void touchEventCancel(NSSet* touches, UIView* view);

        void process();
        
    private:
        static const uint32_t InitialInputReserve = 64;
        
        using InputHandleService = HandleService<Input, InputHandle>;
        
        using InputVector = std::vector<InputHandle>;
        using InputMap = std::unordered_map<ObjectId, InputHandle>;
        using InputVectorIterator = InputVector::iterator;
        
        InputHandleService      handleService_;
        InputVector             inputs_;
        
        Input *createInput();
        void touchEvent(TouchType type, NSSet* touches, UIView* view);
    };
}

#endif /* InputService_h */
