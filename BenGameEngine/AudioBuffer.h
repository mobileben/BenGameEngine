//
//  AudioBuffer.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 11/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef AudioBuffer_h
#define AudioBuffer_h

#include <stdio.h>
#import <Foundation/Foundation.h>
#include <functional>
#include "Error.h"
#include "Handle.h"
#include "NamedObject.h"

#if TARGET_OS_IPHONE
#include "AudioToolbox/AudioFile.h"
#include "AudioToolbox/AudioQueue.h"
#endif /* TARGET_OS_IPHONE */

namespace BGE {
    typedef enum {
        AudioBufferErrorNone = 0,
        AudioBufferErrorOS,
        AudioBufferErrorNoBuffer,
        AudioBufferErrorUnsupportedType,
        AudioBufferErrorMemory
    } AudioBufferError;
    
    enum class AudioFormat {
        PCMMono8,
        PCMMono16,
        PCMStereo8,
        PCMSterio16,
        WAV,
        CAF,
        MP3,
        AAC
    };
    
    class Audio;
    
    class AudioBuffer : public NamedObject {
    public:
        static const std::string ErrorDomain;
        
        AudioBuffer();

        ~AudioBuffer() {}
        
        void initialize(AudioBufferHandle handle, const std::string& name);
        void destroy();
        
        AudioBufferHandle getHandle() const { return handle_; }
        
        void createFromFile(std::string filename, bool streaming, std::function<void(AudioBuffer *, std::shared_ptr<Error>)>callback);
        
        size_t getMemoryUsage();
        double  getDuration() const { return duration_; }
        
    private:
        friend Audio;
        
        AudioBufferHandle           handle_;
        bool                        valid_;
        bool                        streaming_;
        double                      duration_;
#if TARGET_OS_IPHONE
        AudioFileID                 audioFileId_;
        AudioStreamBasicDescription streamBasicDesc_;
        uint32_t                    maxPacketSize_;
        uint8_t                     *audioBuffer_;
        uint32_t                    audioBufferSize_;
#endif /* TARGET_OS_IPHONE */
        
        size_t                      memUsage_;
    };
}

#endif /* AudioBuffer_h */
