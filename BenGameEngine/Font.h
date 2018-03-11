//
//  Font.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEFont_h
#define BGEFont_h

#include <stdio.h>
#include <string>
#include <map>
#include "FontGlyph.h"
#include "TextureAtlas.h"
#include "MathTypes.h"
#include "Handle.h"

namespace BGE {
    class TransformComponent;
    class TextComponent;
    
    typedef enum {
        FontErrorNone = 0,
        FontErrorOS,
        FontErrorFreeType,
        FontErrorAllocation,
        FontErrorNotInTable,
        FontErrorNoResourceFile,
        FontErrorInvalidSubTexture,
        FontErrorExistingTextureWrongType,
    } FontError;

    enum class FontHorizontalAlignment {
        Left,
        Center,
        Right
    };
    
    enum class FontVerticalAlignment {
        Top,
        Center,
        Bottom,
        Baseline
    };
    
    enum class FontStatus {
        Invalid,
        Loading,
        Valid
    };
    
    class Font : public NamedObject
    {
    public:
        static const std::string ErrorDomain;

        Font();
        Font(ObjectId fontId);
        Font(const std::string& name, uint32_t pixelSize);
        virtual ~Font() {}
        
        void initialize(FontHandle handle, const std::string& name, uint32_t pixelSize);
        void destroy();
        
        FontHandle getHandle() const { return handle_; }
        TextureAtlasHandle getTextureAtlasHandle() const { return textureAtlasHandle_; }
        uint32_t getGlyphW() const { return glyphW_; }
        uint32_t getGlyphH() const { return glyphH_; }
        int32_t getBaseline() const { return baseline_; }
        
#ifdef NOT_YET
        std::shared_ptr<FontGlyph> glyphForExtendedASCII(uint16_t code);
#endif
        
        std::string getNameAsKey() const;
        
        bool isValid(void) const { return status_ == FontStatus::Valid; }
        bool hasKerning() const { return hasKerning_; }
        int32_t kerningForPair(uint16_t prev, uint16_t curr);
        uint32_t getStringWidth(std::string str, bool minimum=true);
        uint32_t getHeight() const;
        
        virtual void load(std::string filename, uint32_t faceIndex, std::function<void(FontHandle, std::shared_ptr<Error>)> callback);

        // We allow glyphs_ to be public here so the renderer can access it directly
        std::map<uint16_t, FontGlyph>                       glyphs_;

    protected:
        uint32_t    pixelSize_;
        std::string family_;
        std::string style_;
        FontHandle  handle_;
        FontStatus  status_;
        
        uint32_t glyphW_;   // Max width of glyphs
        uint32_t glyphH_;   // Max height of glyphs
        int32_t baseline_;   // Baseline Y position. Hang is below (negative). Bearing Y is above (positive).
        
        bool hasKerning_;
        
        TextureAtlasHandle                                  textureAtlasHandle_;
        std::map<std::pair<uint16_t, uint16_t>, int32_t>    kerning_;

    private:
        friend class FontService;
    };
}

#endif /* BGEFont_h */
