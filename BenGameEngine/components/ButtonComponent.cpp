//
//  ButtonComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/7/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ButtonComponent.h"
#include "Game.h"

static const char *ButtonStateDisabledString = "disabled";
static const char *ButtonStateDisabledAnimString = "disabled-anim";
static const char *ButtonStateNormalString = "normal";
static const char *ButtonStateNormalAnimString = "normal-anim";
static const char *ButtonStateHighlightedString = "highlighted";
static const char *ButtonStateHighlightedAnimString = "highlighted-anim";

uint32_t BGE::ButtonComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::ButtonComponent::type_index_ = typeid(BGE::ButtonComponent);

std::shared_ptr<BGE::ButtonComponent> BGE::ButtonComponent::create(ObjectId componentId) {
    return std::make_shared<ButtonComponent>(private_key{}, componentId);
}

BGE::ButtonComponent::ButtonComponent(struct private_key const& key, ObjectId componentId) : Component(componentId), state(ButtonStateNormal), animate(false), enabled(true), showHighlighted(true), toggleable(false), toggleOn(false) {
}

void BGE::ButtonComponent::setButtonReference(ButtonReference *buttonRef) {
    if (buttonRef) {
        setButtonReference(*buttonRef);
    }
}

void BGE::ButtonComponent::setButtonReference(const ButtonReference &buttonRef) {
    auto space = Game::getInstance()->getSpaceService()->getSpace(getSpaceHandle());
    auto gameObj = getGameObject().lock();
    
    assert(gameObj);
    
    if (gameObj) {
        auto xform = gameObj->getComponent<TransformComponent>();

        assert(xform);

        if (xform) {
            for (auto i=0;i<buttonRef.numStates;i++) {
                auto state = &buttonRef.states[i];
                
                if (!strcmp(ButtonStateDisabledString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        disabledButton = space->createAnimSequence(state->reference);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        disabledButton = space->createSprite(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(disabledButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateDisabledAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        disabledAnimButton = space->createAnimSequence(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(disabledAnimButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateNormalString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        normalButton = space->createAnimSequence(state->reference);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        normalButton = space->createSprite(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(normalButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateNormalAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        normalAnimButton = space->createAnimSequence(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(normalAnimButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateHighlightedString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        highlightedButton = space->createAnimSequence(state->reference);
                    } else if (state->referenceType == GfxReferenceTypeSprite) {
                        highlightedButton = space->createSprite(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(highlightedButton->getComponent<TransformComponent>());
                } else if (!strcmp(ButtonStateHighlightedAnimString, state->state)) {
                    if (state->referenceType == GfxReferenceTypeAnimationSequence) {
                        highlightedAnimButton = space->createAnimSequence(state->reference);
                    } else {
                        assert(false);
                    }
                    xform->addChild(highlightedAnimButton->getComponent<TransformComponent>());
                }
            }
            
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
                currentButton->setActive(true);
            }
        }
    }
}

std::shared_ptr<BGE::BoundingBoxComponent> BGE::ButtonComponent::getBoundingBox() {
    if (currentButton) {
        return currentButton->getComponent<BoundingBoxComponent>();
    } else {
        auto gameObj = getGameObject().lock();
        return gameObj->getComponent<BoundingBoxComponent>();
    }
}

std::shared_ptr<BGE::TransformComponent> BGE::ButtonComponent::getTransform() {
    if (currentButton) {
        return currentButton->getComponent<TransformComponent>();
    } else {
        auto gameObj = getGameObject().lock();
        return gameObj->getComponent<TransformComponent>();
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

bool BGE::ButtonComponent::isEnabled() const {
    return !(state & ButtonStateDisabled);
}

void BGE::ButtonComponent::setEnabled(bool enabled) {
    if (enabled) {
        state &= ~ButtonStateDisabled;
        
        if (toggleable) {
            if (toggleOn) {
                useHighlightedButton();
            } else {
                useNormalButton();
            }
        } else {
            useNormalButton();
        }
    } else {
        state |= ButtonStateDisabled;
        useDisabledButton();
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

void BGE::ButtonComponent::useHighlightedButton() {
    if (showHighlighted) {
        bool playAnim = false;
        
        currentButton->setActive(false);
        
        if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            animator->stop();
        }
        
        currentButton = nullptr;
        
        if (animate) {
            if (highlightedAnimButton && highlightedAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButton = highlightedAnimButton;
                playAnim = true;
            } else if (highlightedButton) {
                currentButton = highlightedButton;
            } else if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButton = normalAnimButton;
                playAnim = true;
            } else if (normalButton) {
                currentButton = normalButton;
            } else {
                assert(false);
            }

        } else {
            currentButton = highlightedButton;
            
            if (!currentButton) {
                // Fallback as we don't have this
                if (highlightedAnimButton && highlightedAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                    currentButton = highlightedAnimButton;
                    playAnim = true;
                } else if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                    currentButton = normalAnimButton;
                    playAnim = true;
                } else if (normalButton) {
                    currentButton = normalButton;
                } else {
                    assert(false);
                }
            }
        }
        
        if (currentButton) {
            currentButton->setActive(true);
            
            if (playAnim) {
                auto animator = currentButton->getComponent<AnimatorComponent>();
                
                if (animator->state == AnimState::Done) {
                    animator->play(AnimatorComponent::AnimPlayForever);
                }
            }
        }
    }
}

void BGE::ButtonComponent::useDisabledButton() {
    bool playAnim = false;
    
    currentButton->setActive(false);
    
    if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
        auto animator = currentButton->getComponent<AnimatorComponent>();
        animator->stop();
    }
    
    currentButton = nullptr;

    if (animate) {
        if (disabledAnimButton && disabledAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButton = disabledAnimButton;
            playAnim = true;
        } else if (disabledButton) {
            currentButton = disabledButton;
        } else if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButton = normalAnimButton;
            playAnim = true;
        } else if (normalButton) {
            currentButton = normalButton;
        } else {
            assert(false);
        }
    } else {
        currentButton = disabledButton;
        
        if (!currentButton) {
            // Fallback as we don't have this
            if (disabledAnimButton && disabledAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButton = disabledAnimButton;
                playAnim = true;
            } else if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButton = normalAnimButton;
                playAnim = true;
            } else if (normalButton) {
                currentButton = normalButton;
            } else {
                assert(false);
            }
        }
    }
    
    if (currentButton) {
        currentButton->setActive(true);
        
        if (playAnim) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            
            if (animator->state == AnimState::Done) {
                animator->play(AnimatorComponent::AnimPlayForever);
            }
        }
    }
}

void BGE::ButtonComponent::useNormalButton() {
    bool playAnim = false;
    
    currentButton->setActive(false);
    
    if (currentButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
        auto animator = currentButton->getComponent<AnimatorComponent>();
        animator->stop();
    }
    
    currentButton = nullptr;

    if (animate) {
        if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
            currentButton = normalAnimButton;
            playAnim = true;
        } else {
            if (normalButton) {
                currentButton = normalButton;
            } else {
                assert(false);
            }
        }
    } else {
        currentButton = normalButton;
        
        if (!currentButton) {
            // Fallback as we don't have this
            if (normalAnimButton && normalAnimButton->getComponentBitmask() & Component::getBitmask<AnimatorComponent>()) {
                currentButton = normalAnimButton;
                playAnim = true;
            } else {
                assert(false);
            }
        }
    }
    
    if (currentButton) {
        currentButton->setActive(true);
        
        if (playAnim) {
            auto animator = currentButton->getComponent<AnimatorComponent>();
            
            if (animator->state == AnimState::Done) {
                animator->play(AnimatorComponent::AnimPlayForever);
            }
        }
    }
}

BGE::Event BGE::ButtonComponent::handleInput(TouchType type, bool inBounds) {
    Event event = Event::None;
    
    switch (type) {
        case TouchType::Down:
            event = handleTouchDownEvent(inBounds);
            break;
            
        case TouchType::Up:
            event = handleTouchUpEvent(inBounds);
            break;
            
        case TouchType::Cancel:
            event = handleTouchCancelEvent();
            break;
            
        default:
            break;
    }
    
    return event;
}


BGE::Event BGE::ButtonComponent::handleTouchDownEvent(bool inBounds) {
    Event event = Event::None;
    
    if (inBounds) {
        if (isEnabled()) {
            setHighlighted(true);
            event = Event::TouchDown;
        }
    }
    
    return event;
}

BGE::Event BGE::ButtonComponent::handleTouchCancelEvent() {
    Event event = Event::TouchCancel;
    
    useNormalButton();
    
    return event;
}

BGE::Event BGE::ButtonComponent::handleTouchUpEvent(bool inBounds) {
    Event event = Event::None;
    
    if (isEnabled()) {
        if (inBounds) {
            if (toggleable) {
                if (toggleOn) {
                    setToggleOn(false);
                } else {
                    setToggleOn(true);
                }
                
                if (toggleOn) {
                    useHighlightedButton();
                } else {
                    useNormalButton();
                }
            } else {
                setHighlighted(false);
            }
            
            event = Event::TouchUpInside;
        } else {
            setHighlighted(false);
            
            event = Event::TouchUpOutside;
        }
    }
    
    return event;
}

