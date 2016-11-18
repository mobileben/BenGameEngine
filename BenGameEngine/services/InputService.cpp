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

void BGE::InputService::garbageCollect() {
    handleService_.garbageCollect();
    eventService_->garbageCollect();
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
    lock();
    
    auto handle = eventService_->createEventHandlerHandle(gameObj, event, function);
    std::vector<EventHandlerHandle> &v = inputEventHandlers_[event];
    
    v.push_back(handle);
    
    unlock();
    
    return handle;
}

void BGE::InputService::unregisterEventHandler(EventHandlerHandle handle) {
    if (handle.isNull()) {
        return;
    }
    
    lock();
    
    // Remove event handle from vector
    for (auto &mapIt : inputEventHandlers_) {
        auto it = std::find(mapIt.second.begin(), mapIt.second.end(), handle);
        
        if (it != mapIt.second.end()) {
            mapIt.second.erase(it);
            eventService_->removeEventHandler(handle);
            break;
        }
    }
    
    unlock();
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
                
                if (gameObj) {
//                    printf("WARNING: removing input handler for space %s, gameObj %s\n", space->getName().c_str(), gameObj->getName().c_str());
                }
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

bool BGE::InputService::checkInput(Input *input, GameObject *gameObj, std::vector<InputEventItem> &queue) {
    auto xform = gameObj->getComponent<TransformComponent>();
    bool found = false;
    
    if (xform && xform->isVisible()) {
        auto button = gameObj->getComponent<ButtonComponent>();
        
        if (button && button->isEnabled()) {
            // Compute collision if exists
            auto bbox = button->getBoundingBox();
            
            if (bbox) {
                Matrix4 matrix;
                bool inBounds = false;
                
                xform->getWorldMatrix(matrix);
                bbox->computeAABB(matrix, xform->getCollisionRectScale());
                
                if (input->x >= bbox->aabbMinX && input->x < bbox->aabbMaxX) {
                    if (input->y >= bbox->aabbMinY && input->y < bbox->aabbMaxY) {
                        inBounds = true;
                    }
                }
                
                auto event = button->handleInput(input->type, inBounds);
                
                if (event == Event::TouchDown || event == Event::TouchUpInside) {
                    InputEventItem eventItem;
                    
                    eventItem.spaceHandle = gameObj->getSpace()->getHandle();
                    eventItem.gameObjHandle = gameObj->getHandle();
                    eventItem.buttonComponentHandle = button->getHandle<ButtonComponent>();
                    eventItem.touchType = input->type;
                    eventItem.event = event;
                    eventItem.inBounds = inBounds;
                    
                    queue.push_back(eventItem);
                    found = true;
                }
            }
        } else {
            auto inputTouch = gameObj->getComponent<InputTouchComponent>();
            
            if (inputTouch) {
                auto bbox = gameObj->getComponent<BoundingBoxComponent>();
                
                if (bbox) {
                    Matrix4 matrix;
                    
                    xform->getWorldMatrix(matrix);
                    bbox->computeAABB(matrix, xform->getCollisionRectScale());

                    if (input->x >= bbox->aabbMinX && input->x < bbox->aabbMaxX) {
                        if (input->y >= bbox->aabbMinY && input->y < bbox->aabbMaxY) {
                            if (input->type == TouchType::Down) {
                                InputEventItem eventItem;

                                eventItem.spaceHandle = gameObj->getSpace()->getHandle();
                                eventItem.gameObjHandle = gameObj->getHandle();
                                eventItem.buttonComponentHandle = ButtonComponentHandle();
                                eventItem.touchType = input->type;
                                eventItem.event = Event::TouchDownInside;
                                eventItem.inBounds = false;
                                
                                queue.push_back(eventItem);
                            }
                            
                            found = true;
                        }
                    }
                }
            }
        }
        
        if (!found) {
            // Determine if we have children, if we do process them.
            for (auto i=0;i<xform->getNumChildren();i++) {
                auto childXform = xform->childAtIndex(i);
                if (childXform->hasGameObject()) {
                    auto childObjHandle = childXform->getGameObjectHandle();
                    auto childObj = childXform->getSpace()->getGameObject(childObjHandle);
                    
                    // TODO: Have some better means of identifying the right child. For now brute force it
                    if (childObj) {
                        found = checkInput(input, childObj, queue);
                        
                        if (found) {
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return found;
}

void BGE::InputService::update(double deltaTime) {
    lock();
    
    // Sort inputs
    std::sort(inputs_.begin(), inputs_.end());
    
    std::vector<InputTouchEvent> inputTouchQueue;
    std::vector<InputEventItem> inputEventQueue;
    std::vector<SpaceHandle> spaceHandles = Game::getInstance()->getSpaceService()->getReversedSpaces();

    for (auto input : inputs_) {
        for (auto const &handle : spaceHandles) {
            bool found = false;
            
            auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
            
            if (space && space->isActive() && space->isVisible()) {
                std::vector<GameObject *> objects;
                
                space->getRootGameObjects(objects);
                
                for (auto const &obj : objects) {
                    if (obj && obj->isActive()) {
                        found = checkInput(input, obj, inputEventQueue);
                        
                        if (found) {
                            break;
                        }
                    }
                }
            }
            
            if (found) {
                break;
            }
        }
    }

    for (auto input : inputs_) {
        auto handle = input->getHandle();
        handleService_.release(handle);
    }
    
    inputs_.clear();
    
    auto eventService = eventService_;

    for (auto &item : inputEventQueue) {
        auto space = Game::getInstance()->getSpaceService()->getSpace(item.spaceHandle);
        auto gameObj = space->getGameObject(item.gameObjHandle);
        
        if (gameObj) {
            auto button = space->getComponent<ButtonComponent>(item.buttonComponentHandle.getHandle());
            
            if (button) {
                auto event = item.event;
                
                printf("Processed button %s (%d)\n", gameObj->getName().c_str(), item.touchType);
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
            } else if (item.event == Event::TouchDownInside){
                // InputTouchComponent
                auto event = Event::TouchDownInside;
                
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
            }
        } else {
            // buttonHandlers must be associated with active game objects, but it is possible they are nullptr due to being released during processing
        }
    }

    unlock();
}

