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
#include "Space.h"

BGE::InputService::InputService(std::shared_ptr<EventService> eventService) : handleService_(InitialInputReserve, HandleServiceNoMaxLimit), eventService_(eventService) {
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
    lock();
    
    auto scale = [[UIScreen mainScreen] scale];

    for (UITouch *touch in touches) {
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
    
    unlock();
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

BGE::EventHandlerHandle BGE::InputService::registerEventHandler(GameObject *gameObj, Event event, EventHandlerFunction function) {
    auto handle = eventService_->createEventHandlerHandle(gameObj, event, function);
    std::vector<EventHandlerHandle> &v = inputEventHandlers_[event];
    
    v.push_back(handle);
    
    return handle;
}

void BGE::InputService::unregisterEventHandler(EventHandlerHandle handle) {
    if (handle.isNull()) {
        return;
    }
    
    // Remove event handle from vector
    for (auto &mapIt : inputEventHandlers_) {
        auto it = std::find(mapIt.second.begin(), mapIt.second.end(), handle);
        
        if (it != mapIt.second.end()) {
            mapIt.second.erase(it);
            eventService_->removeEventHandler(handle);
            break;
        }
    }
}

void BGE::InputService::spaceReset(Space *space) {
    lock();
    
    auto eventService = eventService_;
    auto spaceHandle = space->getHandle();
    
    for (auto &mapIt : inputEventHandlers_) {
        for (auto hIt=mapIt.second.begin();hIt!=mapIt.second.end();) {
            auto handle = *hIt;
            auto handler = eventService->getEventHandler(handle);
            
            if (handler->spaceHandle == spaceHandle) {
#if DEBUG
                auto gameObj = space->getGameObject(handler->gameObjHandle);
                printf("WARNING: removing input handler for space %s, gameObj %s\n", space->getName().c_str(), gameObj->getName().c_str());
#endif
                hIt = mapIt.second.erase(hIt);
                eventService->removeEventHandler(handle);
            } else {
                hIt++;
            }
        }
    }
    
    unlock();
}

void BGE::InputService::process() {
    lock();
    
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
    
    auto eventService = eventService_;
    
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
                auto gameObjHandle = gameObj->getHandle();
                
                for (auto const &handle : handles) {
                    auto handler = eventService->getEventHandler(handle);
                    
                    if (handler) {
                        if (gameObjHandle == handler->gameObjHandle) {
                            handler->handler(gameObj, event);
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
                auto gameObjHandle = gameObj->getHandle();
                
                for (auto const &handle : handles) {
                    auto handler = eventService->getEventHandler(handle);
                    
                    if (handler) {
                        if (gameObjHandle == handler->gameObjHandle) {
                            handler->handler(gameObj, event);
                        }
                    }
                }
            }
        } else {
            // buttonHandlers must be associated with active game objects
            assert(false);
        }
    }
    
    unlock();
}

