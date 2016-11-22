//
//  AudioService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AudioService_h
#define AudioService_h

#include <stdio.h>
#include <unordered_map>
#include <vector>

#import <Foundation/Foundation.h>

#ifdef TARGET_OS_IPHONE
#import "AudioServiceHelper.h"
#endif

#include "Audio.h"
#include "AudioBuffer.h"
#include "Handle.h"
#include "HandleService.h"
#include "Service.h"

namespace BGE {
    class EventService;

    class AudioService : public Service {
    public:
        AudioService(std::shared_ptr<EventService> eventService);
        ~AudioService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final;
        void garbageCollect() final;

        Audio *getAudio(AudioHandle handle);
        AudioBufferHandle getAudioBufferHandle(std::string name);
        AudioBuffer *getAudioBuffer(std::string name);
        AudioBuffer *getAudioBuffer(AudioBufferHandle handle);
        
        void removeAudio(AudioHandle handle);
        void removeAudioBuffer(std::string name);
        void removeAudioBuffer(AudioBufferHandle handle);
        
        Audio *createAudio(std::string name, std::string audioBufferName);
        Audio *createAudio(std::string name, AudioBufferHandle audioBufferHandle);
        void createAudioBufferFromFile(std::string name, std::string filename, bool streaming, std::function<void(AudioBuffer *, std::shared_ptr<Error>)> callback);
        
#if TARGET_OS_IPHONE
        
        bool		otherAudioPlayingAtStart(void) { return otherAudioPlayingAtStart_; }
        void		enableExternalAppsAudio(void);
        void		disableExternalAppsAudio(void);

#endif /* TARGET_OS_IPHONE */
        
       
        void		setMasterVolume(uint32_t volume);
        uint32_t	getMasterVolume(void);
        
        inline bool	isSFXEnabled(void) { return sfxEnabled_; }
        void		setSFXEnabled(Boolean enabled);
        
        inline bool	isMusicEnabled(void) { return musicEnabled_; }
        void		setMusicEnabled(Boolean enabled);

    private:
        static const uint32_t InitialAudioReserve = 256;
        static const uint32_t InitialAudioBufferReserve = 128;

        using AudioHandleService = HandleService<Audio, AudioHandle>;
        using AudioBufferHandleService = HandleService<AudioBuffer, AudioBufferHandle>;
        
        AudioService() = delete;
        std::shared_ptr<EventService>   eventService_;
        
        AudioHandleService audioHandleService_;
        AudioBufferHandleService audioBufferHandleService_;
        
        std::vector<AudioHandle> audioHandles_;
        std::unordered_map<std::string, AudioBufferHandle>  audioBufferHandles_;
        
        bool sfxEnabled_;
        bool musicEnabled_;
        
#if TARGET_OS_IPHONE
        AudioServiceHelper  *helper_;
        Boolean             otherAudioPlayingAtStart_;
        NSString            *category_;
#endif /* _PLATFORM_IPHONE */

    };
}

#endif /* AudioService_h */
