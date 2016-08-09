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

        Font(ObjectId fontId);
        Font(std::string name, uint32_t pixelSize);
        virtual ~Font();
        
        void initialize(FontHandle handle, ObjectId fontId, std::string name, uint32_t pixelSize);
        
        FontHandle getHandle() const { return handle_; }
        uint32_t getGlyphW() const { return glyphW_; }
        uint32_t getGlyphH() const { return glyphH_; }
        int32_t getBaseline() const { return baseline_; }
        
        std::shared_ptr<FontGlyph> glyphForExtendedASCII(uint16_t code);
        
        std::string getNameAsKey() const;
        
        bool isValid(void) const { return valid_; }
        bool hasKerning() const { return hasKerning_; }
        int32_t kerningForPair(uint16_t prev, uint16_t curr);
        uint32_t getStringWidth(std::string str, bool minimum=true);
        uint32_t getHeight() const;
        
        virtual void load(std::string filename, uint32_t faceIndex, std::function<void(FontHandle, std::shared_ptr<Error>)> callback);

        // TODO: Determine if font rendering should be done else where, like in the renderer versus the font.
        // TODO: We will want to cache the width/height of the string. So this is probably not the best place for the drawString to exist
        // TODO: Move to renderer
        virtual void drawString(std::string str, std::shared_ptr<TransformComponent> transform, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
        virtual void drawString(std::string str, Vector2 &position, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);
        
    protected:
        uint32_t    pixelSize_;
        std::string family_;
        std::string style_;
        FontHandle  handle_;
        FontStatus  status_;
        
        // TODO: Remove valid
        bool valid_;
        
        uint32_t glyphW_;   // Max width of glyphs
        uint32_t glyphH_;   // Max height of glyphs
        int32_t baseline_;   // Baseline Y position. Hang is below (negative). Bearing Y is above (positive).
        
        bool hasKerning_;
        
        std::shared_ptr<TextureAtlas> textureAtlas_;
        std::map<uint16_t, std::shared_ptr<FontGlyph>> glyphs_;
        std::map<std::pair<uint16_t, uint16_t>, int32_t> kerning_;
        
        template <typename T>
        std::shared_ptr<T> derived_shared_from_this()
        {
            return std::static_pointer_cast<T>(Object::shared_from_this());
        }

        void drawString(std::string str, const float *rawMatrix, Color &color, FontHorizontalAlignment horizAlignment=FontHorizontalAlignment::Center, FontVerticalAlignment vertAlignment=FontVerticalAlignment::Center, bool minimum=true);

    private:
        friend class FontService;
    };
}

#endif /* BGEFont_h */
