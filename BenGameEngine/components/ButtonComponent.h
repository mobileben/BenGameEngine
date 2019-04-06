//
//  ButtonComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef ButtonComponent_h
#define ButtonComponent_h

#ifdef __APPLE__
// Force include of TargetConditionals to pick up TARGET_OS macros
#include <TargetConditionals.h>
#endif /* __APPLE__ */

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif /* TARGET_OS_IPHONE */
#include <stdio.h>
#include <memory>
#include <chrono>
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
    class InputService;
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
        BoundingBoxComponent *getBoundingBoxLockless(Space *space, GameObject *root);

        bool isAnimating() const;
        void setAnimate(bool animate);
        bool isTouchable() const;
        void setTouchable(bool touchable);
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
        bool isDebouncing() const { return debouncing_; }
        void cancelDebouncing();
        float pressedTime() const;
        float pressedDuration() const;
        bool pressedByDurationTriggered() const;
        
        void activeUpdated(bool active);
        void visibilityUpdated(bool visible);
        
        Event getEvent() const { return event_; }
        
        void setPressedDuration(float duration);
        
        float getTouchUpCollisionScale() const { return touchUpCollisionScale_; }
        void setTouchUpCollisionScale(float scale) { touchUpCollisionScale_ = scale; }
        
        float getDebounceDuration() const { return debounceDuration_; }
        void setDebounceDuration(float duration) { debounceDuration_ = duration; }
        void setDebounceCompletedFunction(std::function<void(GameObject *)> func);

        void stopHighlightedAnim();
        void setHighlightedAnimCompletedFunction(std::function<void(GameObject *)> func);
        
        Event shouldHandleInput(Input *input, bool inBounds);
        
        float getCollisionScale(Input *input) const;
        
        void update(double deltaTime);

#if TARGET_OS_IPHONE
        UITouch *getTouch() const { return touch; }
        void setTouch(UITouch *t) { touch = t; }
#endif /* TARGET_OS_IPHONE */
        
        GameObjectHandle getDisabledButtonHandle() const { return disabledButtonHandle; }
        GameObjectHandle getDisabledAnimButtonHandle() const { return disabledAnimButtonHandle; }
        GameObjectHandle getNormalButtonHandle() const { return normalButtonHandle; }
        GameObjectHandle getNormalAnimButtonHandle() const { return normalAnimButtonHandle; }
        GameObjectHandle getHighlightedButtonHandle() const { return highlightedButtonHandle; }
        GameObjectHandle getHighlightedAnimButtonHandle() const { return highlightedAnimButtonHandle; }
        
        Event handleTouchInsideForDurationEvent(bool inBounds);

    protected:
        void destroyFast() final;
        
    private:
        friend Space;
        friend InputService;
        friend ComponentService;

        uint32_t    state;
        bool        animate;
        bool        touchable_;
        bool        enabled;
        bool        showHighlighted_;

        // For when we want to treat this like a toggle
        bool        toggleable;
        bool        toggleOn;
        
        Event       event_;
        bool        debouncing_;
        float       debounceDuration_;
        float       debounceTimer_;
        
        float       pressedTime_;
        float       pressedDuration_;
        bool        pressedByDurationTriggered_;
        float       touchUpCollisionScale_;
        
        std::function<void(GameObject *)> debounceCompleted_;
        std::function<void(GameObject *)> highlightedAnimCompleted_;

#if TARGET_OS_IPHONE
        UITouch     *touch; // We need this to properly track events
#endif /* TARGET_OS_IPHONE */
        
        GameObjectHandle disabledButtonHandle;
        GameObjectHandle disabledAnimButtonHandle;
        GameObjectHandle normalButtonHandle;
        GameObjectHandle normalAnimButtonHandle;
        GameObjectHandle highlightedButtonHandle;
        GameObjectHandle highlightedAnimButtonHandle;
        GameObjectHandle currentButtonHandle;
        
        EventHandlerHandle highlightedAnimHandlerHandle;
  
        void useHighlightedButton();        // This never animates
        void useDisabledButton();
        void useNormalButton();

        void animateHighlightedButton();    // This animates
        
        bool serviceDebounce(double deltaTime, bool forceClear=false);
        
        // TODO: If multi-touch then we will need to match touches, if necessary
        Event shouldHandleTouchDownEvent(bool inBounds);
        Event shouldHandleTouchCancelEvent();
        Event shouldHandleTouchMoveEvent(bool inBounds);
        Event shouldHandleTouchUpEvent(bool inBounds);
        
        Event handleTouchDownEvent(bool inBounds);
        Event handleTouchCancelEvent();
        Event handleTouchUpEvent(bool inBounds);
        
        void handleHighlightedAnimEnd();
        void handleHighlightedAnimEndHandler(GameObject *gameObj, Event event);
    };
}

#endif /* ButtonComponent_h */
