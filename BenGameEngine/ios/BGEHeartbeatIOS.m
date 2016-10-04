//
//  BGEHeartbeatIOS.m
//  GamePlayground
//
//  Created by Benjamin Lee on 2/28/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import "BGEHeartbeatIOS.h"

@implementation BGEHeartbeatIOS

- (id)init
{
    self = [super init];
    
    if (self) {
        [self enteringForeground];
    }
    
    return self;
}

- (void)tick:(CADisplayLink *)displayLink
{
    if (self.tickHandler) {
        self.tickHandler();
    }
}

- (void)enteringBackground {
    [_displayLink invalidate];
    _displayLink = nil;
}

- (void)enteringForeground {
    if (!_displayLink) {
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
}

@end
