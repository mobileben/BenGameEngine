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
    
    
    class AudioBuffer;
    
    class Audio : public NamedObject {
    public:
        Audio() {}
        ~Audio() {}
        
        void initialize(AudioHandle handle, std::string name, AudioBuffer *audioBuffer);
        void destroy();
        
        AudioHandle getHandle() const { return handle_; }
        AudioBufferHandle getBufferHandle() const { return audioBufferHandle_; }
        
        AudioPlayState getState() const { return state_; }
        void setState(AudioPlayState state) { state_ = state;}
        
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
        
        inline uint32_t				getNumActualAudioQueueBuferRefUsed(void) const { return actualBuffersUsed_; }
        inline AudioQueueBufferRef	getAudioQueueBufferRefAtIndex(uint32_t index) { return buffers_[index]; }
        
        inline uint8_t              *getAudioMemoryImage(void) const { return audioBuffer_; }
        inline uint32_t				getAudioMemoryImageSize(void) const { return audioBufferSize_; }
        
        void						prime(void);

#endif /* TARGET_OS_IPHONE */
        
        bool isPlaying() const;
        bool isPaused(AudioPauseSource source=AudioPauseSource::None) const;
        bool isLooping() const;
        inline uint32_t	getLoopingCount(void) const { return looping_; }
        inline void setLoopingCount(uint32_t looping) { looping_ = looping; }

        void play(uint32_t loop = AudioPlayOnce);
        void pause(AudioPauseSource source=AudioPauseSource::None);
        void resume(AudioPauseSource source=AudioPauseSource::None);
        void stop();
        
        AudioType getType() const { return type_; }
        
    private:
        AudioHandle                     handle_;
        bool                            valid_;
        AudioPlayState                  state_;
        AudioType                       type_;
        AudioBufferHandle               audioBufferHandle_;

        bool                            streaming_;
        
        uint32_t                        looping_;
        AudioPauseSource                pauseSource_;
        
#if TARGET_OS_IPHONE
        AudioFileID                     audioFileId_;
        uint8_t                         *audioBuffer_;
        uint32_t                        audioBufferSize_;

        AudioQueueRef                   queue_;
        uint32_t                        actualBuffersUsed_;
        AudioQueueBufferRef				buffers_[kAudioQueueNumBuffers];
        int32_t						bufferSize_;
        int32_t							currPacket_;
        int32_t                        numPacketsToRead_;
        AudioStreamPacketDescription    *packetDesc_;
        AudioStreamBasicDescription     streamBasicDesc_;
        int32_t							memoryImageIndex_;
#endif /* TARGET_OS_IPHONE */
    };
}

#endif /* Audio_h */
