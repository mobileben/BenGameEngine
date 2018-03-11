//
//  AudioBuffer.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "AudioBuffer.h"

const std::string BGE::AudioBuffer::ErrorDomain = "AudioBuffer";

BGE::AudioBuffer::AudioBuffer() : valid_(false), streaming_(false), duration_(0), memUsage_(0)
#if TARGET_OS_IPHONE
    , audioFileId_(nullptr), maxPacketSize_(0), audioBuffer_(nullptr), audioBufferSize_(0)
#endif /* TARGET_OS_IPHONE */
{
#if TARGET_OS_IPHONE
    memset(&streamBasicDesc_, 0, sizeof(streamBasicDesc_));
#endif /* TARGET_OS_IPHONE */
}

void BGE::AudioBuffer::initialize(AudioBufferHandle handle, const std::string& name) {
    setName(name);
    handle_ = handle;
    valid_ = false;
    streaming_ = false;
    duration_ = 0;
#if TARGET_OS_IPHONE
    audioFileId_ = nullptr;
    audioBuffer_ = nullptr;
#endif /* TARGET_OS_IPHONE */
}

void BGE::AudioBuffer::destroy() {
#if TARGET_OS_IPHONE
    if (audioBuffer_) {
        free(audioBuffer_);
        audioBuffer_ = nullptr;
    }
    
    if (audioFileId_) {
        AudioFileClose(audioFileId_);
        audioFileId_ = nullptr;
    }
#endif /* TARGET_OS_IPHONE */
}

size_t BGE::AudioBuffer::getMemoryUsage() {
    return memUsage_;
}

void BGE::AudioBuffer::createFromFile(std::string filename, bool streaming, std::function<void(AudioBuffer *, std::shared_ptr<Error>)>callback) {
    streaming_ = streaming;
    
#if TARGET_OS_IPHONE
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(){
        NSString *path = [[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding];
        NSURL *url = [NSURL fileURLWithPath:path];
        std::shared_ptr<Error> error = nullptr;
        
        auto result = AudioFileOpenURL((__bridge CFURLRef)url, kAudioFileReadPermission, 0, &audioFileId_);
        
        if (!result) {
            UInt32 dataSize = sizeof(streamBasicDesc_);
            AudioFileGetProperty(audioFileId_, kAudioFilePropertyDataFormat, &dataSize, &streamBasicDesc_);
            
            dataSize = sizeof(maxPacketSize_);
            
            AudioFileGetProperty(audioFileId_, kAudioFilePropertyPacketSizeUpperBound, &dataSize, &maxPacketSize_);

            Float64 duration = 0;
            dataSize = sizeof(duration);

            AudioFileGetProperty(audioFileId_, kAudioFilePropertyEstimatedDuration, &dataSize, &duration);

            duration_ = duration;

            if (!streaming_) {
                uint64_t bufSize;

                dataSize = sizeof(bufSize);
                
                if (!AudioFileGetProperty(audioFileId_, kAudioFilePropertyAudioDataByteCount, &dataSize, &bufSize)) {
                    UInt32 bytesRead = static_cast<UInt32>(bufSize);
                    
                    audioBuffer_ = (uint8_t *)malloc(bytesRead);
                    
                    if (audioBuffer_) {
                        if (!AudioFileReadBytes(audioFileId_, false, 0, &bytesRead, audioBuffer_)) {
                            if (bufSize == bytesRead) {
                                audioBufferSize_ = bytesRead;
                                memUsage_ = bytesRead;
                                valid_ = true;
                            } else {
                                error = std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorOS);
                            }
                        } else {
                            error = std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorOS);
                        }
                    } else {
                        error = std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorMemory);
                    }
                } else {
                    error = std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorOS);
                }

                AudioFileClose(audioFileId_);
                audioFileId_ = nullptr;
            } else {
                valid_ = true;
            }
        } else {
            error = std::make_shared<Error>(AudioBuffer::ErrorDomain, AudioBufferErrorOS);
        }
        
        if (callback) {
            callback(this, error);
        }
    });
#else
#error Unsupported target
#endif /* TARGET_OS_IPHONE */
}

