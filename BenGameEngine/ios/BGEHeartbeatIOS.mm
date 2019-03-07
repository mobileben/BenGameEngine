//
//  BGEHeartbeatIOS.m
//  GamePlayground
//
//  Created by Benjamin Lee on 2/28/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import "BGEHeartbeatIOS.h"

#include "Game.h"

@implementation BGEHeartbeatIOS

- (id)init
{
    self = [super init];
    
    if (self) {
        [self platformResuming];
    }
    
    return self;
}

- (void)tick:(__attribute__ ((unused)) CADisplayLink *)displayLink
{
    if (self.tickHandler) {
        self.tickHandler();
    }
}

- (void)platformSuspending {
    [_displayLink invalidate];
    _displayLink = nil;
}

- (void)platformResuming {
    if (!_displayLink) {
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    }
}

- (double)secondsPerFrame {
    auto preferredFramesPerSecond = _displayLink.preferredFramesPerSecond;
    if (!preferredFramesPerSecond) {
        preferredFramesPerSecond = [UIScreen mainScreen].maximumFramesPerSecond;
        if (!preferredFramesPerSecond) {
            // Use 60fps if 0
            preferredFramesPerSecond = 60;
        }
    }
    return 1.0 / static_cast<double>(preferredFramesPerSecond);
}

@end
