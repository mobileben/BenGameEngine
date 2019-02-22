//
//  Audio.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Audio.h"
#include "AudioBuffer.h"
#include "MathTypes.h"

#include "Game.h"

#if TARGET_OS_IPHONE

static void AudioQueueHandleOutputBuffer(void* data, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    BGE::AudioHandle handle = BGE::AudioHandle(static_cast<BGE::HandleBackingType>(reinterpret_cast<intptr_t>(data)));
    BGE::Audio *audio = BGE::Game::getInstance()->getAudioService()->getAudio(handle);
    
    if (audio) {
        if (audio->getState() == BGE::AudioPlayState::Off) {
            return;
        }
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
                    
                    AudioQueueEnqueueBuffer(audio->getAudioQueueRef(), inBuffer, (audio->getAudioStreamPacketDescription() ? numPackets : 0), audio->getAudioStreamPacketDescription());
                    
                    audio->setCurrentPacket(audio->getCurrentPacket() + numPackets);
                } else {
                    if (loopForever || loopCount > 1) {
                        if (loopForever == false) {
                            audio->setLoopingCount(loopCount - 1);
                        }
                        
                        audio->setCurrentPacket(0);
                        AudioQueueHandleOutputBuffer(data, inAQ, inBuffer);
                    } else {
                        audio->setCurrentPacket(0);
                        audio->setLoopingCount(0);
                        AudioQueueStop(audio->getAudioQueueRef(), false);
                        if (audio->doneCallback) {
                            audio->doneCallback(audio);
                        }
                    }
                }
            }
        } else {
            bool needsStop = false;
            auto packetDesc = audio->getAudioStreamBasicDescription();
            int32_t	index = audio->getCurrentMemoryImageIndex();
            int32_t extraXferSize = 0;
            int32_t nextIndex = index;
            UInt32	xferSize = packetDesc->mBytesPerPacket * numPackets;
            UInt32	totalSize = audio->getAudioMemoryImageSize();
            uint8_t *audioData = audio->getAudioMemoryImage();
            uint8_t *dst = (uint8_t *) inBuffer->mAudioData;
            bool go = true;

            if (xferSize > totalSize) {
                xferSize = totalSize;
            }
            if (xferSize > inBuffer->mAudioDataBytesCapacity) {
                xferSize = inBuffer->mAudioDataBytesCapacity;
            }

            if ((index + xferSize) >= totalSize) {
                auto diff = totalSize - index;

                if (loopForever || loopCount > 1) {
                    if (loopForever == false) {
                        audio->setLoopingCount(loopCount - 1);
                    }

                    if (xferSize > diff) {
                        extraXferSize = xferSize - diff;
                        nextIndex = extraXferSize;
                        xferSize = diff;
                    } else {
                        // We happen to perfectly line up to the end of file
                        xferSize = diff;
                        nextIndex = 0;
                    }
                } else {
                    if (diff > 0) {
                        xferSize = diff;
                        nextIndex = 0;
                    } else {
                        go = false;
                    }
                    needsStop = true;
                }
            } else {
                nextIndex = index + xferSize;
            }

            if (go) {
                memcpy((uint8_t*) dst, &audioData[index], xferSize);

                if (extraXferSize) {
                    memcpy((uint8_t*) &dst[xferSize], &audioData[nextIndex], extraXferSize);
                }

                inBuffer->mAudioDataByteSize = xferSize + extraXferSize;
                inBuffer->mPacketDescriptionCount = 0;

                AudioQueueEnqueueBuffer(audio->getAudioQueueRef(), inBuffer, (audio->getAudioStreamPacketDescription() ? numPackets : 0), audio->getAudioStreamPacketDescription());
                audio->setCurrentMemoryImageIndex(nextIndex);
            }

            if (needsStop) {
                audio->setLoopingCount(0);
                audio->setCurrentMemoryImageIndex(0);
                AudioQueueStop(audio->getAudioQueueRef(), false);
                if (audio->doneCallback) {
                    audio->doneCallback(audio);
                }
            }
        }
    }
}

static void AudioQueueIsRunningOutputBuffer(void* data, __attribute__ ((unused)) AudioQueueRef inAQ, __attribute__ ((unused)) AudioQueuePropertyID inID) {
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
                audio->setCurrentPacket(0);
                audio->setState(BGE::AudioPlayState::Off);
                AudioQueueReset(audio->getAudioQueueRef());
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

BGE::Audio::Audio() : doneCallback(nullptr), valid_(false), state_(AudioPlayState::Off), type_(AudioType::SFX), streaming_(false), looping_(0), pauseSource_(static_cast<uint32_t>(AudioPauseSource::None)), playbackRate_(1.0)
#if TARGET_OS_IPHONE
, audioFileId_(nullptr), audioBuffer_(nullptr), audioBufferSize_(0), queue_(nullptr), actualBuffersUsed_(0), bufferSize_(0), currPacket_(0), numPacketsToRead_(0), packetDesc_(nullptr), memoryImageIndex_(0)
#endif /* TARGET_OS_IPHONE */
{
#if TARGET_OS_IPHONE
    for (auto i=0;i<kAudioQueueNumBuffers;++i) {
        buffers_[i] = nullptr;
    }

    memset(&streamBasicDesc_, 0, sizeof(streamBasicDesc_));
#endif /* TARGET_OS_IPHONE */
}

void BGE::Audio::initialize(AudioHandle handle, const std::string& name, AudioBuffer *audioBuffer) {
    setName(name);
    handle_ = handle;
    valid_ = false;
    doneCallback = nullptr;

    if (audioBuffer && audioBuffer->valid_) {
        audioBufferHandle_ = audioBuffer->getHandle();
        streaming_ = audioBuffer->streaming_;
#if TARGET_OS_IPHONE
        audioFileId_ = audioBuffer->audioFileId_;
        audioBuffer_ = audioBuffer->audioBuffer_;
        audioBufferSize_ = audioBuffer->audioBufferSize_;
        packetInfo_ = audioBuffer->packetInfo_;
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
                    
                    delete [] magicCookie;
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
                for (i=0;i<kAudioQueueNumBuffers;i++) {
                    status = AudioQueueAllocateBuffer(queue_, bufferSize_, &buffers_[i]);

                    if (status == noErr) {
                        actualBuffersUsed_++;
                    } else {
                        invalid = true;
                        break;
                    }
                }
            }
            
            AudioQueueAddPropertyListener(queue_, kAudioQueueProperty_IsRunning, AudioQueueIsRunningOutputBuffer, reinterpret_cast<void *>(static_cast<intptr_t>(getHandle().getHandle())));
            
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
    if (streaming_) {
        type_ = AudioType::Music;
    } else {
        type_ = AudioType::SFX;
    }
    pauseSource_ = static_cast<uint32_t>(AudioPauseSource::None);
}

void BGE::Audio::destroy() {
    doneCallback = nullptr;

#if TARGET_OS_IPHONE
    if (queue_) {
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
    pauseSource_ = static_cast<uint32_t>(AudioPauseSource::None);
}

void BGE::Audio::setDoneCallback(std::function<void(Audio *)> callback) {
    doneCallback = callback;
}

void BGE::Audio::clearDoneCallback() {
    doneCallback = nullptr;
}

double BGE::Audio::getDuration() const {
    auto buffer = Game::getInstance()->getAudioService()->getAudioBuffer(audioBufferHandle_);
    if (buffer) {
        return buffer->getDuration();
    }
    return 0;
}

bool BGE::Audio::isPlaying() const {
    return (state_ != AudioPlayState::Off && state_ != AudioPlayState::Stopping);
}

bool BGE::Audio::isPaused() const {
    return (state_ == AudioPlayState::Paused);
}

bool BGE::Audio::isPausedForSource(AudioPauseSource source) const {
    uint32_t src = static_cast<uint32_t>(source);
    if (!(pauseSource_ & src)) {
        return false;
    }
    
    return (state_ == AudioPlayState::Paused);
}

bool BGE::Audio::isLooping() const {
    return looping_ > 1;
}

void BGE::Audio::play(uint32_t loop) {
    if (!valid_) {
        return;
    }

    if (loop == 0 || isPlaying()) {
        return;
    }

    looping_ = loop;
    pauseSource_ = static_cast<uint32_t>(AudioPauseSource::None);

#if TARGET_OS_IPHONE
    state_= AudioPlayState::Queued;
    prime();
    AudioQueuePrime(queue_, 0, NULL);
    AudioQueueStart(queue_, NULL);
#else
    state_= AudioPlayState::Playing;
#endif /* TARGET_OS_IPHONE */
}

void BGE::Audio::pauseForSource(AudioPauseSource source) {
    if (!valid_) {
        return;
    }

    if (isPlaying()) {
        if (!isPaused()) {
#if TARGET_OS_IPHONE
            AudioQueuePause(queue_);
#endif /* TARGET_OS_IPHONE */
        }
        state_ = AudioPlayState::Paused;
        pauseSource_ |= static_cast<uint32_t>(source);
    }
}

void BGE::Audio::resumeForSource(AudioPauseSource source) {
    if (!valid_) {
        return;
    }

    uint32_t src = static_cast<uint32_t>(source);
    if (!(pauseSource_ & src)) {
        return;
    }

    pauseSource_ &= ~src;
    
    if (pauseSource_ == static_cast<uint32_t>(AudioPauseSource::None)) {
        state_ = AudioPlayState::Playing;

#if TARGET_OS_IPHONE
        AudioQueueStart(queue_, NULL);
#endif /* TARGET_OS_IPHONE */
    }
}

void BGE::Audio::stop() {
    if (!valid_) {
        return;
    }

    if (isPlaying()) {
        state_ = AudioPlayState::Stopping;
        pauseSource_ = static_cast<uint32_t>(AudioPauseSource::None);
        
#if TARGET_OS_IPHONE
        AudioQueueStop(queue_, YES);
        AudioQueueReset(queue_);
#endif /* TARGET_OS_IPHONE */
    }
}

void BGE::Audio::setEnablePlaybackRate(bool enable) {
#if TARGET_OS_IPHONE
    UInt32 trueValue = enable ? 1 : 0;
    AudioQueueSetProperty(queue_, kAudioQueueProperty_EnableTimePitch, &trueValue, sizeof(trueValue));
    UInt32 propValue = !trueValue;
    AudioQueueSetProperty(queue_, kAudioQueueProperty_TimePitchBypass, &propValue, sizeof(propValue));
    UInt32 timePitchAlgorithm = kAudioQueueTimePitchAlgorithm_Spectral; // supports rate and pitch
    AudioQueueSetProperty(queue_, kAudioQueueProperty_TimePitchAlgorithm, &timePitchAlgorithm, sizeof(timePitchAlgorithm));
#endif /* TARGET_OS_IPHONE */
    enablePlaybackRate_ = enable;
}

void BGE::Audio::setPlaybackRate(float rate) {
#if TARGET_OS_IPHONE
    if (rate < 0.5) {
        rate = 0.5F;
    } else if (nearlyEqual(rate, 1.0F)) {
        rate = 1.0F;
    } else if (rate > 2.0) {
        rate = 2.0F;
    }

    if (playbackRate_ != rate) {
        AudioQueueSetParameter(queue_, kAudioQueueParam_PlayRate, rate);
    }
#endif
    playbackRate_ = rate;
}

void BGE::Audio::setVolume(float vol) {
#if TARGET_OS_IPHONE
    if (vol < 0.0F) {
        vol = 0.0F;
    } else if (vol > 1.0F) {
        vol = 1.0F;
    }
    AudioQueueSetParameter(queue_, kAudioQueueParam_Volume, vol);
#endif
    volume_ = vol;
}

#if TARGET_OS_IPHONE
void BGE::Audio::prime(void) {
    currPacket_ = 0;
    memoryImageIndex_ = 0;
    
    for (auto i=0u;i<actualBuffersUsed_;i++) {
        AudioQueueHandleOutputBuffer(reinterpret_cast<void *>(static_cast<intptr_t>(getHandle().getHandle())), queue_, buffers_[i]);
    }
}
#endif /* TARGET_OS_IPHONE */

