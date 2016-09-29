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
#include "Input.h"
#include "Event.h"
#include "HandleService.h"

namespace BGE {
    enum ButtonState : uint32_t {
        ButtonStateNormal = 0,
        ButtonStateHighlighted = 1 << 0,
        ButtonStateDisabled = 1 << 1,
        ButtonStateSelected = 1 << 2
    };
    
    class BoundingBoxComponent;
    class Input;
    class Space;
    
    class ButtonComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;

        ButtonComponent();
        ~ButtonComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        void destroy() final;

        void setButtonReference(ButtonReference *buttonRef);
        void setButtonReference(const ButtonReference &buttonRef);

        // This may be considered unconventional in that we are allowing us to get another component from this, however it allows us to properly
        // get the most updated BBOX info
        BoundingBoxComponent *getBoundingBox();
        TransformComponent *getTransform();
        
        bool isAnimating() const;
        void setAnimate(bool animate);
        bool isEnabled() const;
        void setEnabled(bool enabled);
        bool isHighlighted() const;
        void setHighlighted(bool highlighted);
        bool showHighlighted() const;
        void setShowHighlighted(bool show);
        bool isSelected() const;
        void setSelected(bool selected);
        bool isToggleOn() const;
        void setToggleOn(bool on);
        bool isToggleable() const;
        void setToggleable(bool on);
        
        Event handleInput(TouchType type, bool inBounds);
        
    private:
        friend Space;
        
        uint32_t    state;
        bool        animate;
        bool        enabled;
        bool        showHighlighted_;
        
        // For when we want to treat this like a toggle
        bool        toggleable;
        bool        toggleOn;
        
        GameObjectHandle disabledButtonHandle;
        GameObjectHandle disabledAnimButtonHandle;
        GameObjectHandle normalButtonHandle;
        GameObjectHandle normalAnimButtonHandle;
        GameObjectHandle highlightedButtonHandle;
        GameObjectHandle highlightedAnimButtonHandle;
        GameObjectHandle currentButtonHandle;
  
        void useHighlightedButton();
        void useDisabledButton();
        void useNormalButton();

        // TODO: If multi-touch then we will need to match touches, if necessary
        Event handleTouchDownEvent(bool inBounds);
        Event handleTouchCancelEvent();
        Event handleTouchUpEvent(bool inBounds);
    };
}

#endif /* ButtonComponent_h */
