//
//  Audio.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Audio.h"
#include "AudioBuffer.h"

#include "Game.h"

#if TARGET_OS_IPHONE

static void AudioQueueHandleOutputBuffer(void* data, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    BGE::AudioHandle handle = BGE::AudioHandle(static_cast<BGE::HandleBackingType>(reinterpret_cast<intptr_t>(data)));
    BGE::Audio *audio = BGE::Game::getInstance()->getAudioService()->getAudio(handle);
    
    if (audio) {
        UInt32      numBytesReadFromFile = 0;
        UInt32      numPackets = audio->getNumPacketsToRead();
        OSStatus	status;
        uint32_t	loopCount = audio->getLoopingCount();
        bool		loopForever = (loopCount == BGE::AudioPlayForever);
        
        if (audio->isStreaming()) {
            numBytesReadFromFile = audio->getAudioMemoryImageSize();
            status = AudioFileReadPacketData(audio->getAudioFileId(), false, &numBytesReadFromFile, audio->getAudioStreamPacketDescription(), audio->getCurrentPacket(), &numPackets, inBuffer->mAudioData);
            
            if (status == noErr) {
                if (numPackets > 0) {
                    inBuffer->mAudioDataByteSize = numBytesReadFromFile;
                    inBuffer->mPacketDescriptionCount = numPackets;
                    
                    status = AudioQueueEnqueueBuffer(audio->getAudioQueueRef(), inBuffer, (audio->getAudioStreamPacketDescription() ? numPackets : 0), audio->getAudioStreamPacketDescription());
                    
                    audio->setCurrentPacket(audio->getCurrentPacket() + numPackets);
                } else {
                    if (loopForever || loopCount > 1) {
                        if (loopForever == false) {
                            audio->setLoopingCount(loopCount - 1);
                        }
                        
                        audio->setCurrentPacket(0);
                        AudioQueueHandleOutputBuffer(data, inAQ, inBuffer);
                    } else {
                        audio->setLoopingCount(0);
                        AudioQueueFlush(audio->getAudioQueueRef());
                        AudioQueueStop(audio->getAudioQueueRef(), false);
                    }
                }
            }
        } else {
            int32_t	index = audio->getCurrentMemoryImageIndex();
            int32_t	xferSize;
            int32_t	totalSize = audio->getAudioMemoryImageSize();
            uint8_t *audioData = audio->getAudioMemoryImage();
            uint8_t *dst = (uint8_t *) inBuffer->mAudioData;
            bool go = true;
            
            if (index >= totalSize) {
                if (loopForever || loopCount > 1) {
                    if (loopForever == false) {
                        audio->setLoopingCount(loopCount - 1);
                    }
                    
                    audio->setCurrentMemoryImageIndex(0);
                    
                    index = audio->getCurrentMemoryImageIndex();
                } else {
                    audio->setLoopingCount(0);
                    
                    AudioQueueFlush(audio->getAudioQueueRef());
                    AudioQueueStop(audio->getAudioQueueRef(), false);
                    
                    go = false;
                }
            }
            
            if (go) {
                xferSize = totalSize - index;
                
                if (xferSize > inBuffer->mAudioDataBytesCapacity) {
                    xferSize = inBuffer->mAudioDataBytesCapacity;
                }
                
                if (xferSize < totalSize) {
                    memcpy((uint8_t*) dst, &audioData[index], xferSize);	//	This implies we are using a queue size that will require multiple loads
                }
                
                inBuffer->mAudioDataByteSize = xferSize;
                inBuffer->mPacketDescriptionCount = 0;
                
                status = AudioQueueEnqueueBuffer(audio->getAudioQueueRef(), inBuffer, 0, audio->getAudioStreamPacketDescription());
                
                audio->setCurrentMemoryImageIndex(index + xferSize);
            }
        }
    }
}

static void AudioQueueIsRunningOutputBuffer(void* data, AudioQueueRef inAQ, AudioQueuePropertyID inID) {
    BGE::AudioHandle handle = BGE::AudioHandle(static_cast<BGE::HandleBackingType>(reinterpret_cast<intptr_t>(data)));
    BGE::Audio *audio = BGE::Game::getInstance()->getAudioService()->getAudio(handle);

    if (audio) {
        switch (audio->getState())
        {
            case BGE::AudioPlayState::Off:
            case BGE::AudioPlayState::Queued:
            case BGE::AudioPlayState::Paused:
                audio->setState(BGE::AudioPlayState::Playing);
                break;
                
            default:
                audio->setState(BGE::AudioPlayState::Off);
                break;
        }
    }
}

static void DeriveBufferSize (AudioStreamBasicDescription &ASBDesc,
                              int32_t                      maxPacketSize,
                              NSTimeInterval                     seconds,
                              int32_t                      *outBufferSize,
                              int32_t                      *outNumPacketsToRead
) {
    static const int maxBufferSize = 0x50000;
    static const int minBufferSize = 0x4000;
    
    if (ASBDesc.mFramesPerPacket != 0) {
        NSTimeInterval numPacketsForTime =
        ASBDesc.mSampleRate / ASBDesc.mFramesPerPacket * seconds;
        *outBufferSize = numPacketsForTime * maxPacketSize;
    } else {
        *outBufferSize =
        maxBufferSize > maxPacketSize ?
        maxBufferSize : maxPacketSize;
    }
    
    if (*outBufferSize > maxBufferSize && *outBufferSize > maxPacketSize) {
        *outBufferSize = maxBufferSize;
    } else {
        if (*outBufferSize < minBufferSize) {
            *outBufferSize = minBufferSize;
        }
    }
    
    *outNumPacketsToRead = *outBufferSize / maxPacketSize;
}

#endif /* TARGET_OS_IPHONE */

void BGE::Audio::initialize(AudioHandle handle, std::string name, AudioBuffer *audioBuffer) {
    setName(name);
    handle_ = handle;
    valid_ = false;
    
    if (audioBuffer && audioBuffer->valid_) {
        audioBufferHandle_ = audioBuffer->getHandle();
        streaming_ = audioBuffer->streaming_;
#if TARGET_OS_IPHONE
        audioFileId_ = audioBuffer->audioFileId_;
        audioBuffer_ = audioBuffer->audioBuffer_;
        audioBufferSize_ = audioBuffer->audioBufferSize_;
        memcpy(&streamBasicDesc_, &audioBuffer->streamBasicDesc_, sizeof(streamBasicDesc_));
        
        OSStatus status;
        uint32_t i;
        bool isFormatVBR;
        UInt32 cookieSize(sizeof(UInt32));
        bool noProperty;
        char* magicCookie = NULL;
        bool invalid = false;
        
        status = AudioQueueNewOutput(&streamBasicDesc_, AudioQueueHandleOutputBuffer, reinterpret_cast<void *>(static_cast<intptr_t>(getHandle().getHandle())), NULL, NULL, 0, &queue_);
        
        if (status == noErr) {
            DeriveBufferSize(streamBasicDesc_, audioBuffer->maxPacketSize_, 0.5, &bufferSize_, &numPacketsToRead_);
            
            isFormatVBR = (streamBasicDesc_.mBytesPerPacket == 0 || streamBasicDesc_.mFramesPerPacket == 0);
            
            if (isFormatVBR) {
                packetDesc_ = new AudioStreamPacketDescription[numPacketsToRead_];
            }
            else {
                packetDesc_ = nullptr;
            }
            
            noProperty = AudioFileGetPropertyInfo(getAudioFileId(), kAudioFilePropertyMagicCookieData, &cookieSize, nullptr);
            
            if (noProperty && cookieSize) {
                magicCookie = new char[cookieSize];
                
                if (magicCookie) {
                    AudioFileGetProperty(getAudioFileId(), kAudioFilePropertyMagicCookieData, &cookieSize, magicCookie);
                    
                    AudioQueueSetProperty(queue_, kAudioQueueProperty_MagicCookie, magicCookie, cookieSize);
                    
                    delete magicCookie;
                } else {
                    invalid = true;
                }
            }
            
            if (isStreaming()) {
                actualBuffersUsed_ = 0;
                
                // When streaming, set audioBufferSize_ to value of bufferSize_ so we know how much buffer to stream
                audioBufferSize_ = bufferSize_;
                
                for (i=0;i<kAudioQueueNumBuffers;i++) {
                    status = AudioQueueAllocateBuffer(queue_, bufferSize_, &buffers_[i]);
                    
                    if (status == noErr) {
                        actualBuffersUsed_++;
                    } else {
                        invalid = true;
                        break;
                    }
                }				
            } else {
                actualBuffersUsed_ = 1;
                bufferSize_ = audioBufferSize_;
                
                status = AudioQueueAllocateBuffer(queue_, bufferSize_, &buffers_[0]);
                memcpy((uint8_t *) buffers_[0]->mAudioData, audioBuffer_, bufferSize_);
                
                buffers_[1] = nullptr;
                buffers_[2] = nullptr;
            }
            
            status = AudioQueueAddPropertyListener(queue_, kAudioQueueProperty_IsRunning, AudioQueueIsRunningOutputBuffer, reinterpret_cast<void *>(static_cast<intptr_t>(getHandle().getHandle())));
            
            if (!invalid) {
                valid_ = true;
            } else {
                
            }
        }
#endif /* TARGET_OS_IPHONE */
    } else {
        audioBufferHandle_ = AudioBufferHandle();
        streaming_ = false;
#if TARGET_OS_IPHONE
        audioFileId_ = nullptr;
        audioBuffer_ = nullptr;
        audioBufferSize_ = 0;
        queue_ = nullptr;
        packetDesc_ = nullptr;
        
        for (auto i=0;i<kAudioQueueNumBuffers;i++) {
            buffers_[i] = nullptr;
        }
#endif /* TARGET_OS_IPHONE */
    }
    
    state_ = AudioPlayState::Off;
    type_ = AudioType::SFX;
    pauseSource_ = AudioPauseSource::None;
}

void BGE::Audio::destroy() {
#if TARGET_OS_IPHONE
    if (queue_) {
        AudioQueueFlush(queue_);
        
        if (state_ != AudioPlayState::Off) {
            AudioQueueStop(queue_, YES);
            
            AudioQueueDispose(queue_, YES);
            
            for (auto i=0;i<kAudioQueueNumBuffers;i++) {
                buffers_[i] = nullptr;
            }
        }
        
        queue_ = nullptr;
        
        if (packetDesc_) {
            delete packetDesc_;
            packetDesc_ = nullptr;
        }
    }
#endif /* TARGET_OS_IPHONE */
    
    state_ = AudioPlayState::Off;
    audioBufferHandle_.nullify();
    pauseSource_ = AudioPauseSource::None;
}

bool BGE::Audio::isPlaying() {
    return (state_ != AudioPlayState::Off && state_ != AudioPlayState::Stopping);
}

bool BGE::Audio::isPaused(AudioPauseSource source) {
    if (source != AudioPauseSource::None && source != pauseSource_) {
        return false;
    }
    
    return (state_ == AudioPlayState::Paused);
}
bool BGE::Audio::isLooping() {
    return looping_ > 1;
}

void BGE::Audio::play(uint32_t loop) {
    if (!valid_) {
        return;
    }
    
    if (loop == 0 || isPlaying())
        return;

    looping_ = loop;
    pauseSource_ = AudioPauseSource::None;
    
#if TARGET_OS_IPHONE
    state_= AudioPlayState::Queued;
    
    prime();
    
    auto status = AudioQueuePrime(queue_, 0, NULL);
    
    status = AudioQueueStart(queue_, NULL);
#endif /* TARGET_OS_IPHONE */
}

void BGE::Audio::pause(AudioPauseSource source) {
    if (!valid_) {
        return;
    }

    if (isPlaying()) {
        state_ = AudioPlayState::Paused;
        pauseSource_ = source;
    }
}

void BGE::Audio::resume(AudioPauseSource source) {
    if (!valid_) {
        return;
    }

    if (pauseSource_ == AudioPauseSource::None || source != pauseSource_) {
        return;
    }
    
#if TARGET_OS_IPHONE
    OSStatus	status;
    
    state_ = AudioPlayState::Playing;
    status = AudioQueueStart(queue_, NULL);
#endif /* TARGET_OS_IPHONE */
}

void BGE::Audio::stop() {
    if (!valid_) {
        return;
    }

    if (isPlaying()) {
        state_ = AudioPlayState::Stopping;
        pauseSource_ = AudioPauseSource::None;
#if TARGET_OS_IPHONE
        AudioQueueStop(queue_, YES);
#endif /* TARGET_OS_IPHONE */
    }
}

#if TARGET_OS_IPHONE
void BGE::Audio::prime(void) {
    currPacket_ = 0;
    memoryImageIndex_ = 0;
    
    for (auto i=0;i<actualBuffersUsed_;i++) {
        AudioQueueHandleOutputBuffer(reinterpret_cast<void *>(static_cast<intptr_t>(getHandle().getHandle())), queue_, buffers_[i]);
    }
}
#endif /* TARGET_OS_IPHONE */

