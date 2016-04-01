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
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    
    return self;
}

- (void)tick:(CADisplayLink *)displayLink
{
    if (self.tickHandler) {
        self.tickHandler();
    }
}

@end
