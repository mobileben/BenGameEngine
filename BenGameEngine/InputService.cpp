//
//  InputService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "InputService.h"

BGE::InputService::InputService() :  handleService_(InitialInputReserve, InputHandleService::NoMaxLimit) {
    inputs_.resize(InitialInputReserve);
}

BGE::InputService::~InputService() {
}


BGE::Input *BGE::InputService::createInput() {
    InputHandle handle;
    Input *input = handleService_.allocate(handle);
    
    if (input) {
        input->initialize(handle, getIdAndIncrement());
    }
    
    return input;
}

void BGE::InputService::touchEvent(TouchType type, NSSet* touches, UIView* view) {
    auto scale = [[UIScreen mainScreen] scale];

    for (UITouch *touch in touches)
    {
        Input *input = createInput();
        
        auto p = [touch locationInView:view];
        
        p.x *= scale;
        p.y *= scale;
        
        input->timestamp = [touch timestamp];
        input->touch = touch;
        input->type = type;
        input->tapCount = (uint32_t) [touch tapCount];
        input->x = p.x;
        input->y = p.y;
        
        NSLog(@"XXXXX Touch %f %f", input->x, input->y);
        inputs_.push_back(input->getHandle());
    }
}

void BGE::InputService::touchEventDown(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Down, touches, view);
}

void BGE::InputService::touchEventUp(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Up, touches, view);
}

void BGE::InputService::touchEventMove(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Move, touches, view);
}

void BGE::InputService::touchEventCancel(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Cancel, touches, view);
}

void BGE::InputService::process() {
    for (auto input : inputs_) {
        handleService_.release(input.getHandle());
    }
    
    inputs_.clear();
}

