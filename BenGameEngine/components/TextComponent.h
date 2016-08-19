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
    private:
        struct private_key {};
        friend Component;

    public:
        static std::type_index  type_index_;
        static uint32_t         typeId_;
        static uint32_t         bitmask_;
        
        static std::shared_ptr<TextComponent> create(ObjectId componentId);

        TextComponent(struct private_key const& key, ObjectId componentId);
        ~TextComponent() {}
        
        const std::string& getText() const {
            return text_;
        }
        
        void setText(std::string& text);
        
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
        
        void setTextReference(TextReference *textRef) {
            if (textRef) {
                setTextReference(*textRef);
            }
        }
        
        void setTextReference(const TextReference& textRef);
        
        float getWidth(bool minimum=true);
        
    protected:
        void materialsUpdated();

    private:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;

        std::string             text_;
        Color                   color_;
        FontHandle              fontHandle_;
        FontHorizontalAlignment horizAlignment_;
        FontVerticalAlignment   vertAlignment_;
        
        void getWidthHeight(float &width, float &height);
        void updateBoundingBox();
    };
}

#endif /* TextComponent_h */
