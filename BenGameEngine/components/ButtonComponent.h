//
//  ButtonComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ButtonComponent_h
#define ButtonComponent_h

#include <stdio.h>
#include <memory>
#include "Component.h"
#include "GraphicFormats.h"

namespace BGE {
    enum ButtonState : uint32_t {
        ButtonStateNormal = 0,
        ButtonStateHighlighted = 1 << 0,
        ButtonStateDisabled = 1 << 1,
        ButtonStateSelected = 1 << 2
    };
    
    class ButtonComponent : public Component
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;
        
    public:
        static std::shared_ptr<ButtonComponent> create(ObjectId componentId);
        
        ButtonComponent(struct private_key const& key, ObjectId componentId);
        ~ButtonComponent() {}
        
        void setButtonReference(ButtonReference *buttonRef);
        void setButtonReference(const ButtonReference &buttonRef);

        bool isAnimating() const;
        void setAnimate(bool animate);
        bool isEnabled() const;
        void setEnabled(bool enabled);
        bool isHighlighted() const;
        void setHighlighted(bool highlighted);
        bool isSelected() const;
        void setSelected(bool selected);
        void setToggleOn(bool on);
        
    private:
        uint32_t    state;
        bool        animate;
        bool        enabled;
        bool        showHighlighted;
        
        // For when we want to treat this like a toggle
        bool        toggleable;
        bool        toggleOn;
        
        std::shared_ptr<GameObject> disabledButton;
        std::shared_ptr<GameObject> disabledAnimButton;
        std::shared_ptr<GameObject> normalButton;
        std::shared_ptr<GameObject> normalAnimButton;
        std::shared_ptr<GameObject> highlightedButton;
        std::shared_ptr<GameObject> highlightedAnimButton;
        std::shared_ptr<GameObject> currentButton;
        
        void useHighlightedButton();
        void useDisabledButton();
        void useNormalButton();
    };
}

#endif /* ButtonComponent_h */
