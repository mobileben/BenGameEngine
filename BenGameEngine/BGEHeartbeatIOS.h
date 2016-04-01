//
//  BGEHeartbeatIOS.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/28/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#include <functional>

@interface BGEHeartbeatIOS : NSObject

@property (nonatomic, readonly) CADisplayLink *displayLink;
@property (nonatomic, assign) std::function<void()> tickHandler;

@end
