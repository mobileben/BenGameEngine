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
#include "RenderComponent.h"
#include "Font.h"
#include "GraphicFormats.h"

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class TextComponent : public BGE::RenderComponent
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<TextComponent> create(uint64_t componentId);
        static std::shared_ptr<TextComponent> create(uint64_t componentId, std::string name);
        
        TextComponent(struct private_key const& key, uint64_t componentId);
        TextComponent(struct private_key const& key, uint64_t componentId, std::string name);
        virtual ~TextComponent() {}
        
        const std::string& getText() const {
            return text_;
        }
        
        void setText(std::string& text) {
            text_ = text;
        }
        
        std::shared_ptr<Font> getFont() const {
            return font_;
        }
        
        void setFont(std::shared_ptr<Font> font) {
            font_ = font;
        }
        
        const Color& getColor() const {
            return color_;
        }
        
        void setColor(Color& color) {
            color_ = color;
        }
        
        void setTextReference(TextReference& textRef) {
            text_ = textRef.text;
            color_ = textRef.color;
            horizAlignment_ = textRef.alignment;
            vertAlignment_ = FontVerticalAlignment::Center;
            font_ = textRef.font;
        }
        
        float getWidth(bool minimum=true);
        
    protected:
        TextComponent(uint32_t componentId);
        TextComponent(uint32_t componentId, std::string name);

        void materialsUpdated();

    private:
        friend BGE::ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;
        
        std::string             text_;
        Color                   color_;
        std::shared_ptr<Font>   font_;
        FontHorizontalAlignment horizAlignment_;
        FontVerticalAlignment   vertAlignment_;
        
    };
}

#endif /* TextComponent_h */
