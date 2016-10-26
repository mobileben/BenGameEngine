//
//  RawTexture.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 10/25/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef RawTexture_h
#define RawTexture_h

#include <stdio.h>
#include <stdint.h>

#include <memory>
#include <string>

namespace BGE {
    class RawTexture {
    public:
        enum class Format : uint32_t {
            Undefined = 0,
            RGB8_A8,
            RGB8,
            RGB5_A1,
            A8,
        };
        
        RawTexture() =delete;
        RawTexture(const RawTexture& other) =delete;
        
        RawTexture(Format format, uint32_t width, uint32_t height, unsigned char *buffer);
        ~RawTexture();
        
        static RawTexture *createFromPng(std::string filename);
        
        inline auto getFormat() const { return format_; }
        inline auto getWidth() const { return width_; }
        inline auto getHeight() const { return height_; }
        inline auto getBuffer() const { return buffer_; }

    private:
        Format            format_;
        uint32_t          width_;
        uint32_t          height_;
        unsigned char     *buffer_;
    };
}

#endif /* RawTexture_h */
