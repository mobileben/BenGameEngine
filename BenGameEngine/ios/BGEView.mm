//
//  BGEView.m
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import "BGEView.h"
#include "Game.h"

@implementation BGEView

#pragma mark - Touch Events

- (void)touchesBegan:(NSSet *)touches withEvent:(__attribute__ ((unused)) UIEvent *)event
{
    BGE::Game::getInstance()->getInputService()->touchEventDown(touches, self);
}


// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(__attribute__ ((unused)) UIEvent *)event
{
    BGE::Game::getInstance()->getInputService()->touchEventMove(touches, self);
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet *)touches withEvent:(__attribute__ ((unused)) UIEvent *)event
{
    BGE::Game::getInstance()->getInputService()->touchEventUp(touches, self);
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(__attribute__ ((unused)) UIEvent *)event
{
    BGE::Game::getInstance()->getInputService()->touchEventCancel(touches, self);
}

@end
