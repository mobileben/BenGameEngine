//
//  AudioService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AudioService.h"
#include "EventService.h"

#if TARGET_OS_IPHONE

#include "AudioToolbox/AudioToolbox.h"
#include "AudioToolbox/AudioServices.h"

#import <AVFoundation/AVAudioSession.h>

#endif /* TARGET_OS_IPHONE */

BGE::AudioService::AudioService(std::shared_ptr<EventService> eventService) : eventService_(eventService), audioHandleService_(InitialAudioReserve, HandleServiceNoMaxLimit), audioBufferHandleService_(InitialAudioBufferReserve, HandleServiceNoMaxLimit), sfxEnabled_(true), musicEnabled_(true) {
#if TARGET_OS_IPHONE
    //	First grab an audio session that let's other audio play concurrent with ours!
    category_ = AVAudioSessionCategoryAmbient;
    
    otherAudioPlayingAtStart_ = [AVAudioSession sharedInstance].isOtherAudioPlaying;
    
    [[AVAudioSession sharedInstance] setCategory:category_ withOptions:AVAudioSessionCategoryOptionMixWithOthers error:nil];
    
    [[AVAudioSession sharedInstance] setActive:YES error:nil];

    helper_ = [[AudioServiceHelper alloc] init];
#endif /* TARGET_OS_IPHONE */
}

void BGE::AudioService::update(double deltaTime) {
    
}

void BGE::AudioService::garbageCollect() {
    eventService_->garbageCollect();
}

BGE::Audio *BGE::AudioService::getAudio(AudioHandle handle) {
    return audioHandleService_.dereference(handle);
}

BGE::AudioBufferHandle BGE::AudioService::getAudioBufferHandle(std::string name) {
    auto it = audioBufferHandles_.find(name);
    
    if (it != audioBufferHandles_.end()) {
        return it->second;
    }
    
    return AudioBufferHandle();
}

BGE::AudioBuffer *BGE::AudioService::getAudioBuffer(std::string name) {
    auto it = audioBufferHandles_.find(name);
    
    if (it != audioBufferHandles_.end()) {
        return getAudioBuffer(it->second);
    }
    
    return nullptr;
}

BGE::AudioBuffer *BGE::AudioService::getAudioBuffer(AudioBufferHandle handle) {
    return audioBufferHandleService_.dereference(handle);
}

void BGE::AudioService::removeAudio(AudioHandle handle) {
    audioHandles_.erase(std::remove(audioHandles_.begin(), audioHandles_.end(), handle), audioHandles_.end());
    audioHandleService_.release(handle);
}

void BGE::AudioService::removeAudioBuffer(std::string name) {
    auto it = audioBufferHandles_.find(name);
    
    if (it != audioBufferHandles_.end()) {
        audioBufferHandleService_.release(it->second);

        audioBufferHandles_.erase(it);
    }
}

void BGE::AudioService::removeAudioBuffer(AudioBufferHandle handle) {
    for (auto it=audioBufferHandles_.begin();it!=audioBufferHandles_.end();++it) {
        if (it->second == handle) {
            audioBufferHandles_.erase(it);
            break;
        }
    }
    
    audioBufferHandleService_.release(handle);
}


BGE::Audio *BGE::AudioService::createAudio(std::string name, std::string audioBufferName) {
    auto buffer = getAudioBuffer(audioBufferName);
    
    if (buffer) {
        AudioHandle handle;
        auto audio = audioHandleService_.allocate(handle);
        
        if (audio) {
            audio->initialize(handle, name, buffer);
            
            return audio;
        }
    }
    
    return nullptr;
}

BGE::Audio *BGE::AudioService::createAudio(std::string name, AudioBufferHandle audioBufferHandle) {
    auto buffer = getAudioBuffer(audioBufferHandle);
    
    if (buffer) {
        AudioHandle handle;
        auto audio = audioHandleService_.allocate(handle);
        
        if (audio) {
            audio->initialize(handle, name, buffer);
            
            return audio;
        }
    }
    
    return nullptr;
}

void BGE::AudioService::createAudioBufferFromFile(std::string name, std::string filename, bool streaming, std::function<void(AudioBuffer *, std::shared_ptr<Error>)> callback) {
    auto it = audioBufferHandles_.find(name);
    
    if (it == audioBufferHandles_.end()) {
        AudioBufferHandle handle;
        auto buffer = audioBufferHandleService_.allocate(handle);
        
        if (buffer) {
            buffer->initialize(handle, name);
            
            audioBufferHandles_[name] = handle;
            
            buffer->createFromFile(filename, streaming, [this, name, handle, callback](AudioBuffer *audioBuffer, std::shared_ptr<Error> error) {
                if (error) {
                    // Remove handle as well as erase from buffer
                    audioBufferHandles_.erase(name);
                    
                    if (audioBuffer) {
                        audioBuffer->destroy();
                    }
                    
                    audioBufferHandleService_.release(handle);
                }
                
                if (callback) {
                    callback(audioBuffer, error);
                }
            });
        } else if (callback) {
            callback(nullptr, std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorMemory));
        }
    } else if (callback) {
        auto buffer = getAudioBuffer(it->second);
        
        callback(buffer, nullptr);
    }
}

#if TARGET_OS_IPHONE

void BGE::AudioService::enableExternalAppsAudio(void) {
    if (![category_ isEqualToString:AVAudioSessionCategoryAmbient]) {
        category_ = AVAudioSessionCategoryAmbient;
        [[AVAudioSession sharedInstance] setCategory:category_ withOptions:AVAudioSessionCategoryOptionMixWithOthers error:nil];

    }
}

void BGE::AudioService::disableExternalAppsAudio(void) {
    if ([category_ isEqualToString:AVAudioSessionCategoryAmbient]) {
        category_ = AVAudioSessionCategorySoloAmbient;
        [[AVAudioSession sharedInstance] setCategory:category_ withOptions:AVAudioSessionCategoryOptionMixWithOthers error:nil];
        
        // Change this to allow music to play
        otherAudioPlayingAtStart_ = false;
    }
}

#endif /* TARGET_OS_IPHONE */

