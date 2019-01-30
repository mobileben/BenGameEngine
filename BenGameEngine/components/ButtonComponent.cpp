//
//  ButtonComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ButtonComponent.h"
#include "Game.h"
#include "MathTypes.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"

static const char *ButtonStateDisabledString = "disabled";
static const char *ButtonStateDisabledAnimString = "disabled-anim";
static const char *ButtonStateNormalString = "normal";
static const char *ButtonStateNormalAnimString = "normal-anim";
static const char *ButtonStateHighlightedString = "highlighted";
static const char *ButtonStateHighlightedAnimString = "highlighted-anim";

static const float kDefaultTouchUpCollisionScale = 3;
static const float kDefaultDebounceTime = 0.200;

uint32_t BGE::ButtonComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::ButtonComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::ButtonComponent::type_index_ = typeid(BGE::ButtonComponent);

BGE::ButtonComponent::ButtonComponent() : Component(), state(ButtonStateNormal), animate(false), touchable_(true), enabled(true), showHighlighted_(true), toggleable(false), toggleOn(false), event_(Event::None), debouncing_(false), debounceDuration_(kDefaultDebounceTime), debounceTimer_(0), pressedTime_(0), pressedDuration_(0), pressedByDurationTriggered_(false), touchUpCollisionScale_(kDefaultTouchUpCollisionScale)
#if TARGET_OS_IPHONE
, touch(nil)
#endif /* TARGET_OS_IPHONE */
{
}

void BGE::ButtonComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    state = ButtonStateNormal;
    animate = false;
    touchable_ = true;
    enabled = true;
    showHighlighted_ = true;
    
    toggleable = false;
    toggleOn = false;
    
    event_ = Event::None;
    debouncing_ = false;
    debounceDuration_ = kDefaultDebounceTime;
    pressedTime_ = 0;
    pressedDuration_ = 0;
    pressedByDurationTriggered_ = false;
    touchUpCollisionScale_ = kDefaultTouchUpCollisionScale;
    
#if TARGET_OS_IPHONE
    touch = nil;
#endif /* TARGET_OS_IPHONE */
    
    disabledButtonHandle = GameObjectHandle();
    disabledAnimButtonHandle = GameObjectHandle();
    normalButtonHandle = GameObjectHandle();
    normalAnimButtonHandle = GameObjectHandle();
    highlightedButtonHandle = GameObjectHandle();
    highlightedAnimButtonHandle = GameObjectHandle();
    currentButtonHandle = GameObjectHandle();
    
    highlightedAnimHandlerHandle = EventHandlerHandle();
    
    pressedTime_ = 0;
}

void BGE::ButtonComponent::destroy() {
    touchable_ = false;
    enabled = false;

#if TARGET_OS_IPHONE
    // Release reference
    touch = nil;
#endif /* TARGET_OS_IPHONE */
    
    auto space = getSpace();
    
    if (space) {
        space->removeGameObject(disabledButtonHandle);
        space->removeGameObject(disabledAnimButtonHandle);
        space->removeGameObject(normalButtonHandle);
        space->removeGameObject(normalAnimButtonHandle);
        space->removeGameObject(highlightedButtonHandle);
        space->removeGameObject(highlightedAnimButtonHandle);
        space->removeGameObject(currentButtonHandle);
        
        Game::getInstance()->getAnimationService()->unregisterEventHandler(highlightedAnimHandlerHandle);
        highlightedAnimHandlerHandle.nullify();
    }

    // Component::destroy last
    Component::destroy();
}

void BGE::ButtonComponent::destroyFast() {
    touchable_ = false;
    enabled = false;

#if TARGET_OS_IPHONE
    // Release reference
    touch = nil;
#endif /* TARGET_OS_IPHONE */

    // Component::destroyFast last
    Component::destroyFast();
}

void BGE::ButtonComponent::setButtonReference(ButtonReference *buttonRef) {
    if (buttonRef) {
        setButtonReference(*buttonRef);
    }
}

void BGE::ButtonComponent::setButtonReference(const ButtonReference &buttonRef) {
    auto space = Game::getInstance()->getSpaceService()->getSpace(getSpaceHandle());
    auto gameObj = space->getGameObject(getGameObjectHandle());
    
    assert(gameObj);
    
    if (gameObj) {
        auto xform = gameObj->getComponent<TransformComponent>();
        GameObject *disabledButton = nullptr;
        GameObject *disabledAnimButton = nullptr;
        GameObject *normalButton = nullptr;
        GameObject *normalAnimButton = nullptr;
        GameObject *highlightedButton = nullptr;
        GameObject *highlightedAnimButton = nullptr;
        GameObject *currentButton = nullptr;
        
        assert(xform);

        if (xform) {
            char catstr[128];
            
            for (auto i=0u;i<buttonRef.numStates;i++) {
                auto state = &buttonRef.states[i];
                if (!strcmp(ButtonStateDisabledString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        disabledButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        disabledButton = space->createSprite(state->reference, catstr, buttonRef.scenePackage);
                    } else {
                        assert(false);
                    }
                    
                    disabledButtonHandle = disabledButton->getHandle();
                    
                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(disabledButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateDisabledAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        disabledAnimButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else {
                        assert(false);
                    }
                    
                    disabledAnimButtonHandle = disabledAnimButton->getHandle();
                    
                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(disabledAnimButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateNormalString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        normalButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        normalButton = space->createSprite(state->reference, catstr, buttonRef.scenePackage);
                    } else {
                        assert(false);
                    }
                    
                    normalButtonHandle = normalButton->getHandle();
                    
                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(normalButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateNormalAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        normalAnimButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else {
                        assert(false);
                    }

                    normalAnimButtonHandle = normalAnimButton->getHandle();

                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(normalAnimButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateHighlightedString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        highlightedButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        highlightedButton = space->createSprite(state->reference, catstr, buttonRef.scenePackage);
                    } else {
                        assert(false);
                    }
                    
                    highlightedButtonHandle = highlightedButton->getHandle();
                    
                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(highlightedButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateHighlightedAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        strcpy(catstr, state->reference);
                        strcat(catstr, state->state);
                        
                        highlightedAnimButton = space->createAnimSequence(state->reference, catstr, buttonRef.scenePackage, nullptr);
                    } else {
                        assert(false);
                    }
                    
                    highlightedAnimButtonHandle = highlightedAnimButton->getHandle();
                    highlightedAnimHandlerHandle = Game::getInstance()->getAnimationService()->registerEventHandler(highlightedAnimButton, BGE::Event::AnimationReachedEnd, std::bind(&ButtonComponent::handleHighlightedAnimEndHandler, this, std::placeholders::_1, std::placeholders::_2));

                    gameObj = space->getGameObject(getGameObjectHandle());
                    xform = gameObj->getComponent<TransformComponent>();
                    xform->addChild(highlightedAnimButton->getComponent<TransformComponent>());
                }
            }
            
            // Initial button from setButtonReference can only be normal or disabled
            normalButton = space->getGameObject(normalButtonHandle);
            normalAnimButton = space->getGameObject(normalAnimButtonHandle);
            disabledButton = space->getGameObject(disabledButtonHandle);
            disabledAnimButton = space->getGameObject(disabledAnimButtonHandle);
            
            if (normalButton) {
                currentButton = normalButton;
                state = ButtonStateNormal;
            } else if (normalAnimButton) {
                currentButton = normalAnimButton;
                state = ButtonStateNormal;
                animate = true;
            } else if (disabledButton) {
                currentButton = disabledButton;
                state = ButtonStateDisabled;
            } else if (disabledAnimButton) {
                currentButton = disabledAnimButton;
                state = ButtonStateDisabled;
                animate = true;
            }
            
            if (currentButton) {
                currentButtonHandle = currentButton->getHandle();
                currentButton->setActive(true);
            }
        }
    }
}

BGE::BoundingBoxComponent *BGE::ButtonComponent::getBoundingBox() {
    auto currentButton = getSpace()->getGameObject(currentButtonHandle);
    
    if (currentButton) {
        return currentButton->getComponent<BoundingBoxComponent>();
    } else {
        auto gameObjHandle = getGameObjectHandle();
        auto gameObj = getSpace()->getGameObject(gameObjHandle);
        
        return gameObj->getComponent<BoundingBoxComponent>();
    }
}

bool BGE::ButtonComponent::isAnimating() const {
    return animate;
}

void BGE::ButtonComponent::setAnimate(bool animate) {
    this->animate = animate;
    
    if (state & (ButtonStateHighlighted | ButtonStateSelected)) {
        useHighlightedButton();
    } else if (state & ButtonStateDisabled) {
        useDisabledButton();
    } else {
        useNormalButton();
    }
}

bool BGE::ButtonComponent::isTouchable() const {
    return touchable_;
}

void BGE::ButtonComponent::setTouchable(bool touchable) {
    touchable_ = touchable;
}

bool BGE::ButtonComponent::isEnabled() const {
    return !(state & ButtonStateDisabled);
}

void BGE::ButtonComponent::setEnabled(bool enabled) {
    if (enabled) {
        state &= ~ButtonStateDisabled;
        
        if (!isDebouncing()) {
            if (toggleable) {
                if (toggleOn) {
                    useHighlightedButton();
                } else {
                    useNormalButton();
                }
            } else {
                useNormalButton();
            }
        }
    } else {
        state |= ButtonStateDisabled;
        if (!isDebouncing()) {
            useDisabledButton();
        }
    }
}

bool BGE::ButtonComponent::isHighlighted() const {
    if (state & ButtonStateHighlighted) {
        return true;
    } else {
        return false;
    }
}

void BGE::ButtonComponent::setHighlighted(bool highlighted) {
    if (toggleable) {
        if (toggleOn) {
            useNormalButton();
        } else {
            useHighlightedButton();
        }
    } else {
        if (highlighted) {
            state |= ButtonStateHighlighted;
            useHighlightedButton();
        } else {
            state &= ButtonStateHighlighted;
            
            // For now, selected acts like highlighted
            if (!(state & ButtonStateSelected)) {
                if (state & ButtonStateDisabled) {
                    useDisabledButton();
                } else {
                    useNormalButton();
                }
            }
        }
    }
}

bool BGE::ButtonComponent::showHighlighted() const {
    return showHighlighted_;
}

void BGE::ButtonComponent::setShowHighlighted(bool show) {
    showHighlighted_ = show;
}

bool BGE::ButtonComponent::isSelected() const {
    if (toggleable) {
        return toggleOn;
    } else {
        if (state & ButtonStateSelected) {
            return true;
        } else {
            return false;
        }
    }
}

void BGE::ButtonComponent::setSelected(bool selected) {
    if (toggleable) {
        toggleOn = selected;
    } else {
        if (selected) {
            state |= ButtonStateSelected;
            
            useHighlightedButton();
        } else {
            state &= ~ButtonStateSelected;
            
            if (!(state & ButtonStateHighlighted)) {
                if (state & ButtonStateDisabled) {
                    useDisabledButton();
                } else {
                    useNormalButton();
                }
            }
        }
    }
}

bool BGE::ButtonComponent::isToggleOn() const {
    return toggleOn;
}

void BGE::ButtonComponent::setToggleOn(bool on) {
    toggleOn = on;
    
    if (toggleable) {
        if (toggleOn) {
            useHighlightedButton();
        } else {
            useNormalButton();
        }
    }
}

bool BGE::ButtonComponent::isToggleable() const {
    return toggleable;
}

void BGE::ButtonComponent::setToggleable(bool on) {
    toggleable = on;
}

float BGE::ButtonComponent::pressedTime() const {
    return pressedTime_;
}

float BGE::ButtonComponent::pressedDuration() const {
    return pressedDuration_;
}

bool BGE::ButtonComponent::pressedByDurationTriggered() const {
    return pressedByDurationTriggered_;
}

void BGE::ButtonComponent::setPressedDuration(float duration) {
    pressedDuration_ = duration;
}

void BGE::ButtonComponent::useHighlightedButton() {
    auto space = getSpace();
    auto currentButton = space->getGameObject(currentButtonHandle);
    
    if (showHighlighted_) {
        bool playAnim = false;
        
        currentButton->setActive(false);
        
        if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            animator->stop();
        }
        
        currentButtonHandle = highlightedButtonHandle;
        currentButton = getSpace()->getGameObject(currentButtonHandle);
        
        if (!currentButton) {
            GameObject *button;
            
            // Fallback as we don't have this
            if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButtonHandle = button->getHandle();
                playAnim = true;
            } else if ((button = space->getGameObject(normalButtonHandle)) != nullptr) {
                currentButtonHandle = button->getHandle();
            } else {
                assert(false);
            }
        }

        currentButton = getSpace()->getGameObject(currentButtonHandle);

        if (currentButton) {
            currentButton->setActive(true);
            
            if (playAnim) {
                auto animator = currentButton->getComponent<AnimatorComponent>();
                
                if (animator->state == AnimState::Done) {
                    animator->play(AnimPlayForever);
                }
            }
        }
    }
}

void BGE::ButtonComponent::useDisabledButton() {
    bool playAnim = false;
    auto space = getSpace();
    auto currentButton = space->getGameObject(currentButtonHandle);
    GameObject *button;

    currentButton->setActive(false);
    
    if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
        auto animator = currentButton->getComponent<AnimatorComponent>();
        animator->stop();
    }
    
    currentButton = nullptr;

    if (animate) {
        if ((button = space->getGameObject(disabledAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButtonHandle = button->getHandle();
            playAnim = true;
        } else if ((button = space->getGameObject(disabledButtonHandle)) != nullptr) {
            currentButtonHandle = button->getHandle();
        } else if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButtonHandle = button->getHandle();
            playAnim = true;
        } else if ((button = space->getGameObject(normalButtonHandle)) != nullptr) {
            currentButtonHandle = button->getHandle();
        } else {
            assert(false);
        }
    } else {
        currentButtonHandle = disabledButtonHandle;

        currentButton = getSpace()->getGameObject(currentButtonHandle);

        if (!currentButton) {
            // Fallback as we don't have this
            if ((button = space->getGameObject(disabledAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButtonHandle = button->getHandle();
                playAnim = true;
            } else if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButtonHandle = button->getHandle();
                playAnim = true;
            } else if ((button = space->getGameObject(normalButtonHandle)) != nullptr) {
                currentButtonHandle = button->getHandle();
            } else {
                assert(false);
            }
        }
    }
    
    currentButton = getSpace()->getGameObject(currentButtonHandle);
    
    if (currentButton) {
        currentButton->setActive(true);
        
        if (playAnim) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            
            if (animator->state == AnimState::Done) {
                animator->play(AnimPlayForever);
            }
        }
    }
}

void BGE::ButtonComponent::useNormalButton() {
    bool playAnim = false;
    auto space = getSpace();
    auto currentButton = space->getGameObject(currentButtonHandle);
    GameObject *button;

    currentButton->setActive(false);
    
    if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
        auto animator = currentButton->getComponent<AnimatorComponent>();
        animator->stop();
    }
    
    currentButton = nullptr;

    if (animate) {
        if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButtonHandle = button->getHandle();
            playAnim = true;
        } else if ((button = space->getGameObject(normalButtonHandle)) != nullptr) {
            currentButtonHandle = button->getHandle();
        } else {
            assert(false);
        }
    } else {
        currentButtonHandle = normalButtonHandle;
        
        currentButton = space->getGameObject(currentButtonHandle);

        if (!currentButton) {
            // Fallback as we don't have this
            if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButtonHandle = button->getHandle();
                playAnim = true;
            } else {
                assert(false);
            }
        }
    }
    
    currentButton = space->getGameObject(currentButtonHandle);
    
    if (currentButton) {
        currentButton->setActive(true);
        
        if (playAnim) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            
            if (animator->state == AnimState::Done) {
                animator->play(AnimPlayForever);
            }
        }
    }
}

void BGE::ButtonComponent::animateHighlightedButton() {
    auto space = getSpace();
    auto currentButton = space->getGameObject(currentButtonHandle);
    
    if (showHighlighted_) {
        bool playAnim = false;
        int32_t count = 1;
        currentButton->setActive(false);
        
        if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            animator->stop();
        }
        
        currentButton = nullptr;
        GameObject *button;
        
        if ((button = space->getGameObject(highlightedAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButtonHandle = button->getHandle();
            playAnim = true;
        } else if ((button = space->getGameObject(highlightedButtonHandle)) != nullptr) {
            currentButtonHandle = button->getHandle();
        } else if ((button = space->getGameObject(normalAnimButtonHandle)) != nullptr && button->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButtonHandle = button->getHandle();
            playAnim = true;
            count = AnimPlayForever;
        } else if ((button = space->getGameObject(normalButtonHandle)) != nullptr) {
            currentButtonHandle = button->getHandle();
        } else {
            assert(false);
        }
        
        currentButton = getSpace()->getGameObject(currentButtonHandle);
        
        if (currentButton) {
            currentButton->setActive(true);
            
            if (playAnim) {
                auto animator = currentButton->getComponent<AnimatorComponent>();
                
                if (animator->state == AnimState::Done) {
                    animator->play(count);
                }
            }
        }
    }
}

BGE:: Event BGE::ButtonComponent::shouldHandleInput(Input *input, bool inBounds) {
    Event event = Event::None;
    
    switch (input->type) {
        case TouchType::Down:
            // For safety anytime we have a touch down, clear our pressedByDurationTriggered_
            pressedByDurationTriggered_ = false;
            event = shouldHandleTouchDownEvent(inBounds);
            
            if (event == Event::TouchDownInside) {
#if TARGET_OS_IPHONE
                touch = input->touch;
#endif /* TARGET_OS_IPHONE */
            }
            break;
            
        case TouchType::Up:
            // Thouch up always handled as something that needs to be done
#if TARGET_OS_IPHONE
            if (touch == input->touch) {
                event = shouldHandleTouchUpEvent(inBounds);
                touch = nil;
            }
#endif /* TARGET_OS_IPHONE */
            break;
            
        case TouchType::Move:
#if TARGET_OS_IPHONE
            if (touch == input->touch) {
                event = shouldHandleTouchMoveEvent(inBounds);
                if (event == Event::TouchInsideForDuration) {
                    touch = nil;
                }
            }
#endif /* TARGET_OS_IPHONE */
            break;
            
        case TouchType::Cancel:
#if TARGET_OS_IPHONE
            // Cancel always treated as somethingthat needs to be done
            if (touch == input->touch) {
                event = shouldHandleTouchCancelEvent();
                touch = nil;
            }
#endif /* TARGET_OS_IPHONE */
            break;
            
        default:
            break;
    }
    
    return event;
}


float BGE::ButtonComponent::getCollisionScale(Input *input) const {
    if ((input->type == TouchType::Move || input->type == TouchType::Up) && touch == input->touch) {
        return touchUpCollisionScale_;
    } else {
        return 1.0;
    }
}

void BGE::ButtonComponent::update(double deltaTime) {
    // Update only used when we're debouncing
    if (debouncing_) {
        if (highlightedAnimButtonHandle.isNull()) {
            debounceTimer_ += deltaTime;
            if (debounceTimer_ >= debounceDuration_) {
                debouncing_ = false;
                pressedByDurationTriggered_ = false;
                event_ = Event::None;
                
                if (isEnabled()) {
                    if (toggleable) {
                        if (toggleOn) {
                            useHighlightedButton();
                        } else {
                            useNormalButton();
                        }
                    } else {
                        setHighlighted(false);
                    }
                } else {
                    useDisabledButton();
                }
            }
        }
    } else if (event_ == Event::TouchDownInside) {
        pressedTime_ += deltaTime;
    }
}

BGE::Event BGE::ButtonComponent::shouldHandleTouchDownEvent(bool inBounds) {
    Event event = Event::None;
    
    if (isTouchable() && isEnabled() && !isDebouncing() && inBounds) {
        event = Event::TouchDownInside;
    }
    
    return event;
}

BGE::Event BGE::ButtonComponent::shouldHandleTouchCancelEvent() {
    // Always cancel
    Event event = Event::TouchCancel;
    return event;
}

BGE::Event BGE::ButtonComponent::shouldHandleTouchMoveEvent(bool inBounds) {
    Event event = Event::None;
    if (isTouchable() && isEnabled() && !isDebouncing()) {
        if (inBounds) {
#ifdef BGE_HANDLE_TOUCH_INSIDE_FOR_DURATION
            if (!pressedByDurationTriggered_ && pressedDuration_ > 0.0) {
                // If we have a pressed duration, then test our pressedTime_
                if (nearlyGreaterThanOrEqual(pressedTime_, pressedDuration_)) {
                    event = Event::TouchInsideForDuration;
                }
            }
#endif /* BGE_HANDLE_TOUCH_INSIDE_FOR_DURATION */
        } else {
            // If we are out of bounds, then reset our pressedTime_
            event = event_;
        }
    }
    
    return event;
}

BGE::Event BGE::ButtonComponent::shouldHandleTouchUpEvent(bool inBounds) {
    Event event = Event::None;
    
    if (isTouchable() && isEnabled() && !isDebouncing()) {
        if (inBounds) {
            event = Event::TouchUpInside;
        } else {
            event = Event::TouchUpOutside;
        }
    }
    
    return event;
}

BGE::Event BGE::ButtonComponent::handleTouchDownEvent(bool inBounds) {
    event_ = Event::None;
    
    pressedTime_ = 0;

    if (isTouchable() && isEnabled() && !isDebouncing()) {
        if (inBounds) {
            setHighlighted(true);
            event_ = Event::TouchDownInside;
        }
    }
    return event_;
}

BGE::Event BGE::ButtonComponent::handleTouchCancelEvent() {
    event_ = Event::TouchCancel;

    pressedTime_ = 0;

    if (isTouchable() && isEnabled() && !isDebouncing()) {
        useNormalButton();
    }

    return event_;
}

BGE::Event BGE::ButtonComponent::handleTouchInsideForDurationEvent(bool inBounds) {
    if (isTouchable() && isEnabled() && !isDebouncing()) {
        if (inBounds && !pressedByDurationTriggered_) {
            if (toggleOn) {
                setToggleOn(false);
            } else {
                setToggleOn(true);
            }
            
            debouncing_ = true;
            debounceTimer_ = 0;
            if (!highlightedAnimButtonHandle.isNull()) {
                animateHighlightedButton();
            }
        } else {
            pressedTime_ = 0;
        }
    } else {
        pressedTime_ = 0;
    }
    
    if (inBounds && !pressedByDurationTriggered_) {
        event_ = Event::TouchInsideForDuration;
        pressedByDurationTriggered_ = true;
    } else {
        // Do nothing (we should not be called)
    }
    
    return event_;
}

BGE::Event BGE::ButtonComponent::handleTouchUpEvent(bool inBounds) {
    event_ = Event::None;
    
    if (isTouchable() && isEnabled() && !isDebouncing()) {
        if (inBounds) {
            if (toggleOn) {
                setToggleOn(false);
            } else {
                setToggleOn(true);
            }
            
            debouncing_ = true;
            debounceTimer_ = 0;
            if (!highlightedAnimButtonHandle.isNull()) {
                animateHighlightedButton();
            }
        } else {
            setHighlighted(false);
        }
    }
    
    if (inBounds) {
        event_ = Event::TouchUpInside;
    } else {
        event_ = Event::TouchUpOutside;
        pressedTime_ = 0;
    }

    return event_;
}

void BGE::ButtonComponent::handleHighlightedAnimEndHandler(__attribute__ ((unused)) GameObject *gameObj, __attribute__ ((unused)) Event event) {
    debouncing_ = false;
    event_ = Event::None;
    
    if (isEnabled()) {
        if (toggleable) {
            if (toggleOn) {
                useHighlightedButton();
            } else {
                useNormalButton();
            }
        } else {
            setHighlighted(false);
        }
    } else {
        useDisabledButton();
    }
}

