//
//  Audio.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Audio_h
#define Audio_h

#include <stdio.h>
#include <Foundation/Foundation.h>
#include "NamedObject.h"
#include "Handle.h"

#include <Foundation/Foundation.h>

#include <functional>

#if TARGET_OS_IPHONE

#include "AudioToolbox/AudioFile.h"
#include "AudioToolbox/AudioQueue.h"

#endif

static const int kAudioQueueNumBuffers = 3;

namespace BGE {
    
    enum class AudioPlayState {
        Off,
        Queued,
        Playing,
        Paused,
        Stopping
    };
    
    enum class AudioType {
        SFX,
        Music
    };
    
    enum class AudioPauseSource : uint32_t {
        None = 0,
        AudioService,
        User1,
        User2
    };

    const uint32_t AudioPlayForever = static_cast<uint32_t>(-1);
    const uint32_t AudioPlayOnce = 1;
    

#if TARGET_OS_IPHONE

    typedef void (*AudioDoneCallbackFunction)(__weak id, SEL, Audio *);

#endif /* TARGET_OS_IPHONE */

    class AudioBuffer;
    
    class Audio : public NamedObject {
    public:
        Audio();

        ~Audio() {}
        
        void initialize(AudioHandle handle, const std::string& name, AudioBuffer *audioBuffer);
        void destroy();
        
        AudioHandle getHandle() const { return handle_; }
        AudioBufferHandle getBufferHandle() const { return audioBufferHandle_; }
        
        AudioPlayState getState() const { return state_; }
        void setState(AudioPlayState state) { state_ = state;}

        void setDoneCallback(std::function<void(Audio *)> callback);
        void clearDoneCallback();

#if TARGET_OS_IPHONE

        inline bool                 isStreaming(void) const { return streaming_; }
        
        inline uint32_t				getNumPacketsToRead(void) const { return numPacketsToRead_; }
        inline AudioFileID			getAudioFileId(void) const { return audioFileId_; }
        inline int32_t				getCurrentPacket(void) { return currPacket_; }
        inline void					setCurrentPacket(int32_t packet) { currPacket_ = packet; }
        inline int32_t				getCurrentMemoryImageIndex(void) const { return memoryImageIndex_; }
        inline void					setCurrentMemoryImageIndex(int32_t index) { memoryImageIndex_ = index; }
        inline AudioQueueRef		getAudioQueueRef(void) { return queue_; }
        inline AudioStreamPacketDescription* getAudioStreamPacketDescription(void) { return packetDesc_; }
        inline AudioStreamBasicDescription *getAudioStreamBasicDescription(void) { return &streamBasicDesc_; }
        inline uint32_t				getNumActualAudioQueueBuferRefUsed(void) const { return actualBuffersUsed_; }
        inline AudioQueueBufferRef	getAudioQueueBufferRefAtIndex(uint32_t index) { return buffers_[index]; }
        
        inline uint8_t              *getAudioMemoryImage(void) const { return audioBuffer_; }
        inline uint32_t				getAudioMemoryImageSize(void) const { return audioBufferSize_; }
        
        void						prime(void);

#endif /* TARGET_OS_IPHONE */

        double getDuration() const;
        
        bool isPlaying() const;
        bool isPaused(AudioPauseSource source=AudioPauseSource::None) const;
        bool isLooping() const;
        inline uint32_t	getLoopingCount(void) const { return looping_; }
        inline void setLoopingCount(uint32_t looping) { looping_ = looping; }

        void play(uint32_t loop = AudioPlayOnce);
        void pause(AudioPauseSource source=AudioPauseSource::None);
        void resume(AudioPauseSource source=AudioPauseSource::None);
        void stop();

        bool getEnablePlaybackRate() const { return enablePlaybackRate_; }
        void setEnablePlaybackRate(bool enable);

        float getPlaybackRate() const { return enablePlaybackRate_ ? playbackRate_ : 1.0F; }
        void setPlaybackRate(float rate);

        float getVolume() const { return volume_; }
        void setVolume(float vol);
        
        AudioType getType() const { return type_; }

        std::function<void(Audio *)>    doneCallback;

    private:
        AudioHandle                     handle_;
        bool                            valid_;
        AudioPlayState                  state_;
        AudioType                       type_;
        AudioBufferHandle               audioBufferHandle_;

        bool                            streaming_;
        
        uint32_t                        looping_;
        AudioPauseSource                pauseSource_;
        bool                            enablePlaybackRate_;
        float                           playbackRate_;

        float                           volume_;

#if TARGET_OS_IPHONE
        AudioFileID                     audioFileId_;
        uint8_t                         *audioBuffer_;
        uint32_t                        audioBufferSize_;

        AudioQueueRef                   queue_;
        uint32_t                        actualBuffersUsed_;
        AudioFilePacketTableInfo        packetInfo_;
        AudioQueueBufferRef				buffers_[kAudioQueueNumBuffers];
        int32_t						    bufferSize_;
        int32_t							currPacket_;
        int32_t                         numPacketsToRead_;
        AudioStreamPacketDescription    *packetDesc_;
        AudioStreamBasicDescription     streamBasicDesc_;
        int32_t							memoryImageIndex_;
#endif /* TARGET_OS_IPHONE */
    };
}

#endif /* Audio_h */
