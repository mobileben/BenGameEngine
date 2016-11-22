//
//  AudioServiceHelper.m
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#import "AudioServiceHelper.h"
#import <AVFoundation/AVAudioSession.h>

#include "Game.h"

@interface AudioServiceHelper()
@end

@implementation AudioServiceHelper

- (instancetype)init {
    self = [super init];
    
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(audioSessionWasInterrupted:) name:AVAudioSessionInterruptionNotification object:nil];
    }
    
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)audioSessionWasInterrupted:(NSNotification *)notification {
    if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
        BGE::Game::getInstance()->getAudioService()->pause();
    } else {
        BGE::Game::getInstance()->getAudioService()->resume();
    }
}

@end
