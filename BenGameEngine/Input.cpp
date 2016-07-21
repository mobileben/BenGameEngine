//
//  Input.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Input.h"

BGE::Input::Input(ObjectId inputId) : Object(inputId)  {
}

void BGE::Input::initialize(InputHandle handle, ObjectId inputId) {
    handle_ = handle;
    setInstanceId(inputId);
    
    type = TouchType::None;
    timestamp = 0;
    touch = nil;
    x = 0;
    y = 0;
    tapCount = 0;
}
