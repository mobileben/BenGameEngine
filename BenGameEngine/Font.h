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

namespace BGE {
    class TransformComponent;
    
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
    
    class Font: public std::enable_shared_from_this<Font>
    {
    public:
        static const std::string ErrorDomain;
        
        Font(std::string name, uint32_t pixelSize);
        virtual ~Font();
        
        uint32_t getGlyphW() const { return glyphW_; }
        uint32_t getGlyphH() const { return glyphH_; }
        int32_t getBaseline() const { return baseline_; }
        
        std::shared_ptr<FontGlyph> glyphForExtendedASCII(uint16_t code);
        
        bool isValid(void) const { return valid_; }
        bool hasKerning() const { return hasKerning_; }
        int32_t kerningForPair(uint16_t prev, uint16_t curr);
        uint32_t getStringWidth(std::string str, bool minimum=true);
        uint32_t getHeight() const;
        
        virtual void load(std::string filename, uint32_t faceIndex, std::function<void(std::shared_ptr<Font>, std::shared_ptr<BGE::Error> error)> callback) =0;
        // TODO: Determine if font rendering should be done else where, like in the renderer versus the font.
        // TODO: We will want to cache the width/height of the string. So this is probably not the best place for the drawString to exist
        virtual void drawString(std::string str, std::shared_ptr<TransformComponent> transform, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true) =0;
        virtual void drawString(std::string str, Vector2 &position, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true) =0;
        
    protected:
        std::string name_;
        uint32_t pixelSize_;
        std::string family_;
        std::string style_;
        
        FontStatus status_;
        bool valid_;
        
        uint32_t glyphW_;   // Max width of glyphs
        uint32_t glyphH_;   // Max height of glyphs
        int32_t baseline_;   // Baseline Y position. Hang is below (negative). Bearing Y is above (positive).
        
        bool hasKerning_;
        
        std::shared_ptr<BGE::TextureAtlas> textureAtlas_;
        std::map<uint16_t, std::shared_ptr<FontGlyph>> glyphs_;
        std::map<std::pair<uint16_t, uint16_t>, int32_t> kerning_;
        
        template <typename T>
        std::shared_ptr<T> derived_shared_from_this()
        {
            return std::static_pointer_cast<T>(shared_from_this());
        }

    private:
        friend class FontService;
    };
}

#endif /* BGEFont_h */
