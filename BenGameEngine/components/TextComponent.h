//
//  TextComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef TextComponent_h
#define TextComponent_h

#include <stdio.h>
#include "RenderService.h"
#include "BoundingBoxComponent.h"
#include "RenderComponent.h"
#include "Font.h"
#include "GraphicFormats.h"

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class TextComponent : public RenderComponent
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        TextComponent();
        ~TextComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
            RenderComponent::initialize(handle, spaceHandle);
            
            text_ = "";
            fontHandle_ = FontHandle();
        }

        void destroy() final {
            text_ = "";
            fontHandle_ = FontHandle();

            // RenderComponent::destroy last
            RenderComponent::destroy();
        }
        
        const std::string& getText() const {
            return text_;
        }
        
        void setText(std::string const &text);
        
        FontHandle getFontHandle() const {
            return fontHandle_;
        }
        
        void setFont(FontHandle fontHandle);
        
        const Color& getColor() const {
            return color_;
        }
        
        void setColor(Color& color) {
            color_ = color;
        }
        
        void setColor(Color&& color) {
            color_ = color;
        }

        const Color& getDropShadowColor() const {
            return dropShadowColor_;
        }

        void setDropShadowColor(Color& color) {
            dropShadowColor_ = color;
        }

        void setDropShadowColor(Color&& color) {
            dropShadowColor_ = color;
        }

        const Vector2& getDropShadowOffset() const {
            return dropShadowOffset_;
        }

        void setDropShadowOffset(Vector2& offset) {
            dropShadowOffset_ = offset;
        }

        void setDropShadowOffset(Vector2&& offset) {
            dropShadowOffset_ = offset;
        }

        void setTextReference(TextReference *textRef) {
            if (textRef) {
                setTextReference(*textRef);
            }
        }
        
        void setTextReference(const TextReference& textRef);
        
        float getWidth(bool minimum=true);
        void getWidthHeight(float &width, float &height);
        float getBoundsWidth() const { return boundsWidth_; }
        float getBoundsHeight() const { return boundsHeight_; }
        
        FontHorizontalAlignment getHorizontalAlignment() const { return horizAlignment_; }
        FontVerticalAlignment getVerticalAlignment() const { return vertAlignment_; }
        
        bool isMultiline() const { return multiline_; }
        void setMultiline(bool multi);

        bool isDropShadow() const { return dropShadow_; }
        void setDropShadow(bool dropShadow) { dropShadow_ = dropShadow; }

        const auto &getMultiText() const { return multiText_; }
        const auto &getMultiTextY() const { return textY_; }
        
    protected:
        void materialsUpdated();

    private:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;

        std::string                 text_;
        std::vector<std::string>    multiText_;
        std::vector<float>          textY_;
        float                       boundsWidth_;
        float                       boundsHeight_;
        Color                       color_;
        Color                       dropShadowColor_;
        Vector2                     dropShadowOffset_;
        FontHandle                  fontHandle_;
        FontHorizontalAlignment     horizAlignment_;
        FontVerticalAlignment       vertAlignment_;
        float                       leading_;
        bool                        multiline_;
        bool                        dropShadow_;
        
        void updateBoundingBox();
        
        void buildLines();
    };
}

#endif /* TextComponent_h */
