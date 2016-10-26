//
//  RawTexture.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 10/25/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "RawTexture.h"
#include "png.h"
#include "stdlib.h"
#include <cassert>

static const int32_t kTextureAlignment = 4;

static const uint32_t kPNGHeaderSize = 8;

BGE::RawTexture *BGE::RawTexture::createFromPng(std::string filename) {
    // PNG header is 8 bytes
    png_byte header[kPNGHeaderSize];
    FILE *fp = fopen(filename.c_str(), "rb");
    auto format = RawTexture::Format::Undefined;
    png_byte *image = nullptr;
    png_byte **rowPtrs = nullptr;
    RawTexture *texture = nullptr;
    
    if (fp) {
        // Read the PNG header
        fread(header, sizeof(png_byte), kPNGHeaderSize, fp);
        
        // Create and init png_struct
        auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        
        if (!png_ptr) {
            fclose(fp);
            return nullptr;
        }
        
        // PNG image info
        auto info_ptr = png_create_info_struct(png_ptr);
        
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
            fclose(fp);
            return texture;
        }
        
        // PNG image info for pre/post IDAT info separation
        auto end_info_ptr = png_create_info_struct(png_ptr);
        
        if (!end_info_ptr) {
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            fclose(fp);
            return texture;
        }
        
        // libpng uses setjmp/longjmp for default error handling
        if (setjmp(png_jmpbuf(png_ptr))) {

            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
            fclose(fp);
            
            if (image) {
                free(image);
            }
            
            if (rowPtrs) {
                free(rowPtrs);
            }
            
            return texture;
        }
        
        // Init png reading
        png_init_io(png_ptr, fp);
        
        // Header has already been consumed
        png_set_sig_bytes(png_ptr, 8);
        
        // read all the info up to the image data
        png_read_info(png_ptr, info_ptr);
        
        int bitDepth;
        int colorType;
        png_uint_32 width;
        png_uint_32 height;
        
        // Get info on png
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr);

        switch (colorType) {
            case PNG_COLOR_TYPE_RGB:
                png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
                format = RawTexture::Format::RGB8;
                break;
                
            case PNG_COLOR_TYPE_RGB_ALPHA:
                format = RawTexture::Format::RGB8_A8;
                break;
                
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                format = RawTexture::Format::A8;
                assert(false); // We don't know if this will work yet
                break;

            case PNG_COLOR_TYPE_GRAY:
                if (bitDepth < 8) {
                    png_set_expand_gray_1_2_4_to_8(png_ptr);
                }
                assert(false); // We don't know if this will work yet
                break;
                
            case PNG_COLOR_TYPE_PALETTE:
                png_set_palette_to_rgb(png_ptr);
                png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
                format = RawTexture::Format::RGB8;
                assert(false); // We don't know if this will work yet
                break;
        }
        
        if (bitDepth == 16) {
            png_set_strip_16(png_ptr);
        }
        
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png_ptr);
        }
        
        // Update the png info struct.
        png_read_update_info(png_ptr, info_ptr);
        
        auto bytesInRow = png_get_rowbytes(png_ptr, info_ptr);
        auto offset = (kTextureAlignment - 1) - static_cast<int32_t>(((bytesInRow - 1) % kTextureAlignment));
        
        if (offset < 0) {
            offset = 0;
        }
        
        bytesInRow += offset;

        if (format != RawTexture::Format::Undefined) {
            image = (png_byte *) malloc(bytesInRow * height * sizeof(png_byte) + 15);
            
            if (image) {
                rowPtrs = (png_byte **) malloc(height * sizeof(png_byte *));
                
                if (rowPtrs) {
                    for (auto i=0;i<height;++i) {
                        rowPtrs[i] = &image[i * bytesInRow];
                    }
                    
                    png_read_image(png_ptr, rowPtrs);
                    
                    texture = new RawTexture(format, width, height, image);

                    free(rowPtrs);
                } else {
                    free(image);
                    image = nullptr;
                }
            }
        }
        
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
        
        fclose(fp);
    }
    
    return texture;
}

BGE::RawTexture::RawTexture(Format format, uint32_t width, uint32_t height, unsigned char *image) : format_(format), width_(width), height_(height), buffer_(image) {
    
}

BGE::RawTexture::~RawTexture() {
    if (buffer_) {
        free(buffer_);
    }
}
