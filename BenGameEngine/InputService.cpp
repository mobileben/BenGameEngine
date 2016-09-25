//
//  InputService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/19/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "InputService.h"
#include "Game.h"
#include "InputTouchComponent.h"
#include "ButtonComponent.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"

BGE::InputService::InputService() :  handleService_(InitialInputReserve, HandleServiceNoMaxLimit) {
    inputs_.reserve(InitialInputReserve);
}

BGE::Input *BGE::InputService::createInput() {
    InputHandle handle;
    Input *input = handleService_.allocate(handle);
    
    if (input) {
        input->initialize(handle);
    }
    
    return input;
}

void BGE::InputService::touchEvent(TouchType type, NSSet* touches, UIView* view) {
    auto scale = [[UIScreen mainScreen] scale];

    for (UITouch *touch in touches)
    {
        Input *input = createInput();
        
        auto p = [touch locationInView:view];
        
        p.x *= scale;
        p.y *= scale;
        
        input->timestamp = [touch timestamp];
        input->touch = touch;
        input->type = type;
        input->tapCount = (uint32_t) [touch tapCount];
        
        // Convert to proper coordinates if needed
        std::shared_ptr<RenderWindow> window = Game::getInstance()->getRenderService()->getRenderWindow();

        switch (Game::getInstance()->getRenderService()->getCoordinateSystem2D()) {
            case Render2DCoordinateSystem::Traditional:
                input->x = p.x;
                input->y = p.y;
                break;
            case Render2DCoordinateSystem::TraditionalCentered:
                input->x = p.x - (window->getWidth() * window->getContentScaleFactor() / 2.0);
                input->y = p.y - (window->getHeight() * window->getContentScaleFactor() / 2.0);
                break;
            case Render2DCoordinateSystem::OpenGL:
                input->x = p.x;
                input->y = (window->getHeight() * window->getContentScaleFactor()) - p.y;
                break;
            case Render2DCoordinateSystem::OpenGLCentered:
                input->x = p.x - (window->getWidth() * window->getContentScaleFactor() / 2.0);
                input->y = (window->getHeight() * window->getContentScaleFactor() / 2.0) - p.y;
                break;
        }
        
        
        NSLog(@"XXXXX Touch %f %f", input->x, input->y);
        inputs_.push_back(input);
    }
}

void BGE::InputService::touchEventDown(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Down, touches, view);
}

void BGE::InputService::touchEventUp(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Up, touches, view);
}

void BGE::InputService::touchEventMove(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Move, touches, view);
}

void BGE::InputService::touchEventCancel(NSSet* touches, UIView* view) {
    touchEvent(TouchType::Cancel, touches, view);
}

BGE::EventHandlerHandle BGE::InputService::registerEventHandler(std::string name, Event event, EventHandlerFunction function) {
    auto handle = Game::getInstance()->getEventService()->createEventHandlerHandle(name, event, function);
    std::vector<EventHandlerHandle> &v = inputEventHandlers_[event];
    
    v.push_back(handle);
    
    return handle;
}

void BGE::InputService::unregisterEventHandler(EventHandlerHandle handle) {
    Game::getInstance()->getEventService()->removeEventHandler(handle);
    
    // Remove event handle from vector
    for (auto &mapIt : inputEventHandlers_) {
        auto it = std::find(mapIt.second.begin(), mapIt.second.end(), handle);
        
        if (it != mapIt.second.end()) {
            mapIt.second.erase(it);
        }
    }
}

void BGE::InputService::process() {
    // Sort inputs
    std::sort(inputs_.begin(), inputs_.end());
    
    std::vector<InputTouchEvent> inputTouchQueue;
    
    inputButtonHandlers_.clear();
    
    for (auto const &input : inputs_) {
        for (auto const &handle : Game::getInstance()->getSpaceService()->getSpaces()) {
            auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
            
            if (space->isVisible()) {
                // Do buttons first
                std::vector<ButtonComponent *> buttonTouchComponents;
                space->getComponents<ButtonComponent>(buttonTouchComponents);
                
                for (auto const &touch : buttonTouchComponents) {
                    if (touch->isEnabled()) {
                        auto gameObjHandle = touch->getGameObjectHandle();
                        
                        // Compute collision if exists
                        auto bbox = touch->getBoundingBox();
                        auto xform = touch->getTransform();
                        
                        Matrix4 matrix;
                        bool inBounds = false;
                        
                        xform->getMatrix(matrix);
                        bbox->computeAABB(matrix);
                        
                        if (input->x >= bbox->aabbMinX && input->x < bbox->aabbMaxX) {
                            if (input->y >= bbox->aabbMinY && input->y < bbox->aabbMaxY) {
                                inBounds = true;
                            }
                        }
                        
                        InputButtonHandler buttonHandler;
                        
                        buttonHandler.spaceHandle = handle;
                        buttonHandler.gameObjHandle = gameObjHandle;
                        buttonHandler.buttonComponentHandle = touch->getHandle<ButtonComponent>();
                        buttonHandler.touchType = input->type;
                        buttonHandler.inBounds = inBounds;
                        
                        inputButtonHandlers_.push_back(buttonHandler);
                    }
                }
                
                // Do input touch components. Only applicable for TouchType::Down
                if (input->type == TouchType::Down) {
                    std::vector<InputTouchComponent *> inputTouchComponents;
                    space->getComponents<InputTouchComponent>(inputTouchComponents);
                    
                    for (auto const &touch : inputTouchComponents) {
                        auto gameObj = touch->getGameObject();
                        
                        if (gameObj->isActive()) {
                            auto xform = gameObj->getComponent<TransformComponent>();
                            auto bbox = gameObj->getComponent<BoundingBoxComponent>();
                            
                            if (xform && bbox) {
                                Matrix4 matrix;
                                
                                xform->getMatrix(matrix);
                                bbox->computeAABB(matrix);
                                
                                if (input->x >= bbox->aabbMinX && input->x < bbox->aabbMaxX) {
                                    if (input->y >= bbox->aabbMinY && input->y < bbox->aabbMaxY) {
                                        InputTouchEvent inputTouchEvent{handle, gameObj->getHandle()};
                                        
                                        inputTouchQueue.push_back(inputTouchEvent);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        input->destroy();
        handleService_.release(input->getHandle().getHandle());
    }
    
    inputs_.clear();
    
    auto eventService = Game::getInstance()->getEventService();
    
    for (auto const &buttonHandler : inputButtonHandlers_) {
        auto space = Game::getInstance()->getSpaceService()->getSpace(buttonHandler.spaceHandle);
        auto gameObj = space->getGameObject(buttonHandler.gameObjHandle);
        
        if (gameObj) {
            auto buttonComponent = space->getComponent<ButtonComponent>(buttonHandler.buttonComponentHandle.getHandle());
            auto event = buttonComponent->handleInput(buttonHandler.touchType, buttonHandler.inBounds);
            // Now if we match an event handler, dispatch that too
            auto it = inputEventHandlers_.find(event);
            
            if (it != inputEventHandlers_.end()) {
                auto const &handles = it->second;
                auto gameObjName = gameObj->getName();
                
                for (auto const &handle : handles) {
                    auto handler = eventService->getEventHandler(handle);
                    
                    if (handler) {
                        std::string name = handler->getName();
                        
                        if (name.length()) {
                            // TODO: Optimize later on without using name
                            if (gameObjName == name) {
                                handler->handler(space->getHandle(), buttonHandler.gameObjHandle, event);
                            }
                        } else {
                            // Un-named handlers always fire
                            handler->handler(space->getHandle(), buttonHandler.gameObjHandle, event);
                        }
                    }
                }
            }
        } else {
            // buttonHandlers must be associated with active game objects
            assert(false);
        }
    }
    
    for (auto const &inputTouchEvent : inputTouchQueue) {
        auto event = Event::TouchDownInside;
        auto space = Game::getInstance()->getSpaceService()->getSpace(inputTouchEvent.spaceHandle);
        auto gameObj = space->getGameObject(inputTouchEvent.gameObjHandle);
        
        if (gameObj) {
            // Now if we match an event handler, dispatch that too
            auto it = inputEventHandlers_.find(event);
            
            if (it != inputEventHandlers_.end()) {
                auto const &handles = it->second;
                auto gameObjName = gameObj->getName();
                
                for (auto const &handle : handles) {
                    auto handler = eventService->getEventHandler(handle);
                    
                    if (handler) {
                        std::string name = handler->getName();
                        
                        if (name.length()) {
                            // TODO: Optimize later on without using name
                            if (gameObjName == name) {
                                handler->handler(inputTouchEvent.spaceHandle, inputTouchEvent.gameObjHandle, event);
                            }
                        } else {
                            // Un-named handlers always fire
                            handler->handler(inputTouchEvent.spaceHandle, inputTouchEvent.gameObjHandle, event);
                        }
                    }
                }
            }
        } else {
            // buttonHandlers must be associated with active game objects
            assert(false);
        }
    }
}

