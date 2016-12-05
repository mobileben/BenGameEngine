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
        
        
#ifdef NOT_YET
        printf("XXXXX Touch %d %f %f\n", type, input->x, input->y);
#endif
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

void BGE::InputService::checkInput(Input *input, GameObject *gameObj, std::vector<InputEventItem> &queue) {
    auto xform = gameObj->getComponent<TransformComponent>();
    
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
                
                auto event = button->shouldHandleInput(input, inBounds);
                
                if (event != Event::None) {
                    InputEventItem eventItem;
                    
                    eventItem.spaceHandle = gameObj->getSpace()->getHandle();
                    eventItem.gameObjHandle = gameObj->getHandle();
                    eventItem.buttonComponentHandle = button->getHandle<ButtonComponent>();
                    eventItem.inputTouchComponentHandle = InputTouchComponentHandle();
                    eventItem.touchType = input->type;
                    eventItem.event = event;
                    eventItem.inBounds = inBounds;
                    
                    queue.push_back(eventItem);
                }
            }
        } else {
            auto inputTouch = gameObj->getComponent<InputTouchComponent>();
            
            if (inputTouch) {
                auto bbox = gameObj->getComponent<BoundingBoxComponent>();
                
                if (bbox) {
                    Matrix4 matrix;
                    bool inBounds = false;
                    
                    xform->getWorldMatrix(matrix);
                    bbox->computeAABB(matrix, xform->getCollisionRectScale());

                    if (input->x >= bbox->aabbMinX && input->x < bbox->aabbMaxX) {
                        if (input->y >= bbox->aabbMinY && input->y < bbox->aabbMaxY) {
                            inBounds = true;
                            
                            if (input->type == TouchType::Down) {
                                InputEventItem eventItem;

                                eventItem.spaceHandle = gameObj->getSpace()->getHandle();
                                eventItem.gameObjHandle = gameObj->getHandle();
                                eventItem.buttonComponentHandle = ButtonComponentHandle();
                                eventItem.inputTouchComponentHandle = InputTouchComponentHandle();
                                eventItem.inputTouchComponentHandle = inputTouch->getHandle<InputTouchComponent>();
                                eventItem.touchType = input->type;
                                eventItem.event = Event::TouchDownInside;
                                eventItem.inBounds = true;

                                queue.push_back(eventItem);
                                inputTouch->touch = input->touch;
                            }
                        }
                    }
                    
                    if (inputTouch->touch == input->touch) {
                        if (input->type == TouchType::Up && inputTouch->touch == input->touch) {
                            auto event = Event::TouchUpOutside;
                            
                            if (inBounds) {
                                event = Event::TouchUpInside;
                            }
                            
                            InputEventItem eventItem;
                            
                            eventItem.spaceHandle = gameObj->getSpace()->getHandle();
                            eventItem.gameObjHandle = gameObj->getHandle();
                            eventItem.buttonComponentHandle = ButtonComponentHandle();
                            eventItem.inputTouchComponentHandle = inputTouch->getHandle<InputTouchComponent>();
                            eventItem.touchType = input->type;
                            eventItem.event = event;
                            eventItem.inBounds = inBounds;

                            queue.push_back(eventItem);
                            
                            inputTouch->touch = nil;
                        } else if (input->type == TouchType::Cancel) {
                            inputTouch->touch = nil;
                        }
                    }
                }
            }
        }
        
        // Determine if we have children, if we do process them.
        for (auto i=0;i<xform->getNumChildren();i++) {
            auto childXform = xform->childAtIndex(i);
            if (childXform->hasGameObject()) {
                auto childObjHandle = childXform->getGameObjectHandle();
                auto childObj = childXform->getSpace()->getGameObject(childObjHandle);
                
                // TODO: Have some better means of identifying the right child. For now brute force it
                if (childObj && childObj->isActive() && childObj->isVisible()) {
                    checkInput(input, childObj, queue);
                }
            }
        }
    }
}

bool compareInputPointers(BGE::Input *lhs, BGE::Input *rhs) {
    return lhs->timestamp < rhs->timestamp;
}

void BGE::InputService::update(double deltaTime) {
    lock();
    
    // Sort inputs
    std::sort(inputs_.begin(), inputs_.end(), compareInputPointers);
    
    std::vector<InputTouchEvent> inputTouchQueue;
    std::vector<InputEventItem> inputEventQueue;
    std::vector<SpaceHandle> spaceHandles = Game::getInstance()->getSpaceService()->getSpaces();

    for (auto input : inputs_) {
        std::vector<InputEventItem> inputQueue;
        
        for (auto const &handle : spaceHandles) {
            auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
            
            if (space && space->isActive() && space->isVisible()) {
                std::vector<GameObject *> objects;
                
                space->getRootGameObjects(objects);
                
                for (auto const &obj : objects) {
                    if (obj && obj->isActive() && obj->isVisible()) {
                        checkInput(input, obj, inputQueue);
                    }
                }
            }
        }
        
        if (inputQueue.size() > 0) {
            auto spaceService = Game::getInstance()->getSpaceService();

            inputEventQueue.push_back(inputQueue.back());
            inputQueue.pop_back();
            
            // Now clear out any touch tags that match
            for (auto &item : inputQueue) {
                auto space = spaceService->getSpace(item.spaceHandle);
                
                if (space) {
                    auto button = space->getComponent(item.buttonComponentHandle);
                    
                    if (button) {
                        if (button->getTouch() == input->touch) {
                            button->setTouch(nil);
                        }
                    } else {
                        auto inputTouch = space->getComponent(item.inputTouchComponentHandle);
                        
                        if (inputTouch) {
                            if (inputTouch->touch == input->touch) {
                                inputTouch->touch = nil;
                            }
                        }
                    }
                }
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
#ifdef NOT_YET
                auto parent = gameObj->getParent();
                std::string parentName = "none";
                
                if (parent) {
                    parentName = parent->getName();
                }
                
                printf("Processed button %s::%s (%d)\n", parentName.c_str(), gameObj->getName().c_str(), item.touchType);
#endif
                switch(event) {
                    case Event::TouchDownInside:
                        button->handleTouchDownEvent(item.inBounds);
                        break;
                        
                    case Event::TouchCancel:
                        button->handleTouchCancelEvent();
                        break;
                        
                    case Event::TouchUpOutside:
                    case Event::TouchUpInside:
                        button->handleTouchUpEvent(item.inBounds);
                        break;
                        
                    default:
                        break;
                }

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

    if (Game::getInstance()->showCollisionRects()) {
        auto &bboxPoints = Game::getInstance()->getRenderService()->getBoundBoxPoints();
        auto &scaledBBoxPoints = Game::getInstance()->getRenderService()->getScaledBoundBoxPoints();
        
        bboxPoints.clear();
        scaledBBoxPoints.clear();
        
        for (auto const &handle : spaceHandles) {
            auto space = Game::getInstance()->getSpaceService()->getSpace(handle);
            
            if (space && space->isActive() && space->isVisible()) {
                std::vector<GameObject *> objects;
                
                space->getRootGameObjects(objects);
                
                for (auto const &obj : objects) {
                    getInputPoints(obj, bboxPoints, scaledBBoxPoints);
                }
            }
        }
    }
    
    unlock();
}

void BGE::InputService::getInputPoints(GameObject *gameObj, std::vector<Vector3>& bboxPoints, std::vector<Vector3>& scaledBBoxPoints) {
    if (gameObj && gameObj->isActive() && gameObj->isVisible()) {
        auto button = gameObj->getComponent<ButtonComponent>();
        auto xform = gameObj->getComponent<TransformComponent>();
        BoundingBoxComponent *bbox = nullptr;
        
        if (button && button->isEnabled()) {
            // Buttons need to check their parent because often button's bezels are the ones not visible
            auto parent = gameObj->getParent();
            
            if (!parent || (parent->isActive() && parent->isVisible())) {
                // Compute collision if exists
                bbox = button->getBoundingBox();
            }
        }
        else {
            auto inputTouch = gameObj->getComponent<InputTouchComponent>();
                
            if (inputTouch) {
                bbox = gameObj->getComponent<BoundingBoxComponent>();
            }
        }
        
        if (bbox) {
            Matrix4 matrix;
            xform->getWorldMatrix(matrix);
            bbox->computeAABB(matrix);
            // Add points
            bboxPoints.push_back(Vector3{ bbox->aabbMinX, bbox->aabbMinY, 0});
            bboxPoints.push_back(Vector3{ bbox->aabbMaxX, bbox->aabbMinY, 0});
            bboxPoints.push_back(Vector3{ bbox->aabbMaxX, bbox->aabbMaxY, 0});
            bboxPoints.push_back(Vector3{ bbox->aabbMinX, bbox->aabbMaxY, 0});
            
            if (xform->useCollisionRectScale()) {
                bbox->computeAABB(matrix, xform->getCollisionRectScale());
                
                // Add points
                scaledBBoxPoints.push_back(Vector3{ bbox->aabbMinX, bbox->aabbMinY, 0});
                scaledBBoxPoints.push_back(Vector3{ bbox->aabbMaxX, bbox->aabbMinY, 0});
                scaledBBoxPoints.push_back(Vector3{ bbox->aabbMaxX, bbox->aabbMaxY, 0});
                scaledBBoxPoints.push_back(Vector3{ bbox->aabbMinX, bbox->aabbMaxY, 0});
            }
        }
        
        for (auto i=0;i<xform->getNumChildren();i++) {
            auto childXform = xform->childAtIndex(i);
            if (childXform->hasGameObject()) {
                auto childObjHandle = childXform->getGameObjectHandle();
                auto childObj = childXform->getSpace()->getGameObject(childObjHandle);
                
                // TODO: Have some better means of identifying the right child. For now brute force it
                if (childObj) {
                    getInputPoints(childObj, bboxPoints, scaledBBoxPoints);
                }
            }
        }
    }
}
