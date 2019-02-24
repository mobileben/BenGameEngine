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
#ifdef SUPPORT_PROFILING
#include "Profiling.h"
#endif /* SUPPORT_PROFILING */

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

#if TARGET_OS_IPHONE

void BGE::InputService::touchEvent(TouchType type, NSSet* touches, UIView* view) {
    lock();
    
    auto scale = [[UIScreen mainScreen] nativeScale];
    auto window = BGE::Game::getInstance()->getRenderService()->getRenderWindow();
    auto mapX = scale * window->getFromMappedXScale();
    auto mapY = scale * window->getFromMappedYScale();
    auto w_2 = window->getMappedWidth() / 2.0;
    auto h = window->getMappedHeight();
    auto h_2 = h / 2.0;
    
    for (UITouch *touch in touches) {
        Input *input = createInput();
        auto p = [touch locationInView:view];
        p.x *= mapX;
        p.y *= mapY;
        
        input->timestamp = [touch timestamp];
        input->touch = touch;
        input->type = type;
        input->tapCount = (uint32_t) [touch tapCount];
        
        // Convert to proper coordinates if needed
        switch (Game::getInstance()->getRenderService()->getCoordinateSystem2D()) {
            case Render2DCoordinateSystem::Traditional:
                input->x = p.x;
                input->y = p.y;
                break;
            case Render2DCoordinateSystem::TraditionalCentered:
                input->x = p.x - w_2;
                input->y = p.y - h_2;
                break;
            case Render2DCoordinateSystem::OpenGL:
                input->x = p.x;
                input->y = h - p.y;
                break;
            case Render2DCoordinateSystem::OpenGLCentered:
                input->x = p.x - w_2;
                input->y = h_2 - p.y;
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

#endif /* TARGET_OS_IPHONE */


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

bool BGE::InputService::getBboxPoints(std::vector<Vector3>& bbox, std::vector<Vector3>& scaledBbox) {
    auto locked = bboxMutex_.try_lock();

    if (locked) {
        bbox = bboxPoints_;
        scaledBbox = scaledBBoxPoints_;
        bboxMutex_.unlock();
    }

    return locked;
}

void BGE::InputService::updateInputObject(Space *space, GameObject *gameObj, double deltaTime) {
    auto xform = gameObj->getComponentLockless<TransformComponent>(space);
    
    if (xform) {
        auto button = gameObj->getComponentLockless<ButtonComponent>(space);
        
        if (button) {
            button->update(deltaTime);
        }
        
        // Determine if we have children, if we do process them.
        uint32_t numChildren = xform->getNumChildren();
        for (uint32_t i=0;i<numChildren;i++) {
            auto childXform = xform->childAtIndexLockless(space, i);
            if (childXform && childXform->hasGameObject()) {
                auto childObj = childXform->getGameObjectLockless(space);
                
                // TODO: Have some better means of identifying the right child. For now brute force it
                if (childObj && childObj->isActive() && childObj->canInteractLockless(space)) {
                    updateInputObject(space, childObj, deltaTime);
                }
            }
        }
    }
}

void BGE::InputService::checkInput(Input *input, Space *space, GameObject *gameObj, std::vector<InputEventItem> &queue) {
    auto xform = gameObj->getComponent<TransformComponent>(space);
    
    if (xform && xform->canInteract()) {
#ifdef SUPPORT_PROFILING
        ++numProcessedObjects_;
#endif /* SUPPORT_PROFILING */
        auto button = gameObj->getComponentLockless<ButtonComponent>(space);
        
        if (button) {
            // Compute collision if exists
            auto bbox = button->getBoundingBox();
            
            if (bbox) {
                Matrix4 matrix;
                bool inBounds = false;
                
                xform->getWorldMatrix(matrix);
                // collision rect based on artwork override bounds, button collision scale based on buttons that allow for allowing for an increased
                // area on touch up
                Vector2 rectScale = xform->getCollisionRectScale();
                auto buttonScale = button->getCollisionScale(input);
                rectScale.x *= buttonScale;
                rectScale.y *= buttonScale;
                bbox->computeAABB(matrix, rectScale);
                
                if (BGE::nearlyGreaterThanOrEqual(input->x, bbox->aabbMinX) && input->x < bbox->aabbMaxX) {
                    if (BGE::nearlyGreaterThanOrEqual(input->y, bbox->aabbMinY) && input->y < bbox->aabbMaxY) {
                        inBounds = true;
                    }
                }

                auto event = button->shouldHandleInput(input, inBounds);
                if (event != Event::None) {
                    InputEventItem eventItem;
                    
                    eventItem.space = space;
#ifdef OBSOLETE
                    eventItem.spaceHandle = space->getHandle();
#endif
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
            auto inputTouch = gameObj->getComponentLockless<InputTouchComponent>(space);
            
            if (inputTouch) {
                auto bbox = gameObj->getComponentLockless<BoundingBoxComponent>(space);
                
                if (bbox) {
                    Matrix4 matrix;
                    bool inBounds = false;
                    
                    xform->getWorldMatrix(matrix);
                    bbox->computeAABB(matrix, xform->getCollisionRectScale());

                    if (BGE::nearlyGreaterThanOrEqual(input->x, bbox->aabbMinX) && input->x < bbox->aabbMaxX) {
                        if (BGE::nearlyGreaterThanOrEqual(input->y, bbox->aabbMinY) && input->y < bbox->aabbMaxY) {
                            inBounds = true;
                            
                            if (input->type == TouchType::Down) {
                                InputEventItem eventItem;

                                eventItem.space = space;
#ifdef OBSOLETE
                                eventItem.spaceHandle = space->getHandle();
#endif
                                eventItem.gameObjHandle = gameObj->getHandle();
                                eventItem.buttonComponentHandle = ButtonComponentHandle();
                                eventItem.inputTouchComponentHandle = InputTouchComponentHandle();
                                eventItem.inputTouchComponentHandle = inputTouch->getHandle<InputTouchComponent>();
                                eventItem.touchType = input->type;
                                eventItem.event = Event::TouchDownInside;
                                eventItem.inBounds = true;

                                queue.push_back(eventItem);
#if TARGET_OS_IPHONE
                                inputTouch->touch = input->touch;
#endif /* TARGET_OS_IPHONE */
                            }
                        }
                    }

#if TARGET_OS_IPHONE
                    if (inputTouch->touch == input->touch) {
                        if (input->type == TouchType::Up && inputTouch->touch == input->touch) {
                            auto event = Event::TouchUpOutside;
                            
                            if (inBounds) {
                                event = Event::TouchUpInside;
                            }
                            
                            InputEventItem eventItem;
                            
                            eventItem.space = space;
#ifdef OBSOLETE
                            eventItem.spaceHandle = space->getHandle();
#endif
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
#endif /* TARGET_OS_IPHONE */
                }
            }
        }
        
        // Determine if we have children, if we do process them.
        uint32_t numChildren = xform->getNumChildren();
        for (uint32_t i=0;i<numChildren;i++) {
            auto childXform = xform->childAtIndex(space, i);
            if (childXform->hasGameObject()) {
                auto childObj = childXform->getGameObjectLockless(space);
                
                // TODO: Have some better means of identifying the right child. For now brute force it
                if (childObj && childObj->isActive() && childObj->canInteractLockless(space)) {
                    checkInput(input, space, childObj, queue);
                }
            }
        }
    }
}

bool compareInputPointers(BGE::Input *lhs, BGE::Input *rhs) {
    return lhs->timestamp < rhs->timestamp;
}

void BGE::InputService::update(double deltaTime) {
//    lock();

#ifdef SUPPORT_PROFILING
    auto startTime = profiling::EpochTime::timeInMicroSec();
    numProcessedObjects_ = 0;
#endif /* SUPPORT_PROFILING */
    // First pass updates any buttons we may have
    // This is needed bacause we debounce buttons by either
    // time or awaiting highlighted anims completing
    auto spaceService = Game::getInstance()->getSpaceService();
    
    spaceService->getSpaces(spaceHandles_);

    handleServicesLock();
    
    auto length = spaceHandles_.size();
    spaces_.clear();
    spaces_.reserve(length);
    rootGameObjects_.reserve(length);
    
    while (rootGameObjects_.size() <= length) {
        rootGameObjects_.push_back(std::vector<GameObject *>());
    }
    
    for (size_t i=0;i<length;++i) {
        auto const& handle = spaceHandles_[i];
        auto space = spaceService->getSpaceLockless(handle);
        spaces_.push_back(space);
        
        if (space && space->isActive() && space->isVisible()) {
            auto& rootGameObjs = rootGameObjects_[i];
            space->getRootGameObjects(rootGameObjs);
            for (auto const &obj : rootGameObjs) {
                updateInputObject(space, obj, deltaTime);
            }
        }
    }

    handleServicesUnlock();
    
    // Sort inputs
    std::sort(inputs_.begin(), inputs_.end(), compareInputPointers);

    inputEventQueue_.clear();
    
    handleServicesLock();
    for (auto input : inputs_) {
        inputEventWorkQueue_.clear();
        for (size_t i=0;i<length;++i) {
            auto space = spaces_[i];
            if (space && space->isActive() && space->isVisible()) {
                //space->getRootGameObjects(rootGameObjects_);
                auto& rootGameObjs = rootGameObjects_[i];

                for (auto const &obj : rootGameObjs) {
                    if (obj->canInteractLockless(space)) {
                        checkInput(input, space, obj, inputEventWorkQueue_);
                    }
                }
            }
        }

        if (inputEventWorkQueue_.size() > 0) {
            inputEventQueue_.push_back(inputEventWorkQueue_.back());
            inputEventWorkQueue_.pop_back();
            
            // Now clear out any touch tags that match
            for (auto &item : inputEventWorkQueue_) {
#ifdef OBSOLETE
                auto space = spaceService->getSpace(item.spaceHandle);
#else
                auto space = item.space;
#endif
                
                if (space) {
                    auto button = space->getComponentLockless(item.buttonComponentHandle);
                    
                    if (button) {
#if TARGET_OS_IPHONE
                        if (button->getTouch() == input->touch) {
                            button->setTouch(nil);
                        }
#endif /* TARGET_OS_IPHONE */
                    } else {
                        auto inputTouch = space->getComponentLockless(item.inputTouchComponentHandle);
                        
                        if (inputTouch) {
#if TARGET_OS_IPHONE
                            if (inputTouch->touch == input->touch) {
                                inputTouch->touch = nil;
                            }
#endif /* TARGET_OS_IPHONE */
                        }
                    }
                }
            }
        }
    }
    handleServicesUnlock();

    for (auto input : inputs_) {
        auto handle = input->getHandle();
        handleService_.release(handle);
    }
    
    inputs_.clear();
    
    auto eventService = eventService_;

    for (auto &item : inputEventQueue_) {
#ifdef OBSOLETE
        auto space = spaceService->getSpace(item.spaceHandle);
#else
        auto space = item.space;
#endif /* OBSOLETE */
        auto gameObj = space->getGameObject(item.gameObjHandle);
        
        if (gameObj) {
            auto button = space->getComponent<ButtonComponent>(item.buttonComponentHandle.getHandle());
            
            if (button && button->isTouchable() && button->isEnabled()) {
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
                        event = button->handleTouchDownEvent(item.inBounds);
                        break;
                        
                    case Event::TouchCancel:
                        event = button->handleTouchCancelEvent();
                        break;
                        
#ifdef BGE_HANDLE_TOUCH_INSIDE_FOR_DURATION
                    case Event::TouchInsideForDuration:
                        event = button->handleTouchInsideForDurationEvent(item.inBounds);
                        break;
#endif /* BGE_HANDLE_TOUCH_INSIDE_FOR_DURATION */
                        
                    case Event::TouchUpOutside:
                    case Event::TouchUpInside:
                        event = button->handleTouchUpEvent(item.inBounds);
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
        std::lock_guard<std::mutex> lock(bboxMutex_);
        bboxPoints_.clear();
        scaledBBoxPoints_.clear();
        
        for (size_t i=0;i<length;++i) {
            auto space = spaces_[i];
            if (space && space->isActive() && space->isVisible()) {
                //space->getRootGameObjects(rootGameObjects_);
                auto& rootGameObjs = rootGameObjects_[i];
                
                for (auto const &obj : rootGameObjs) {
                    getInputPoints(space, obj, bboxPoints_, scaledBBoxPoints_);
                }
            }
        }
    }

#ifdef SUPPORT_PROFILING
    auto now = profiling::EpochTime::timeInMicroSec();
    processingTime_ = now - startTime;
#endif /* SUPPORT_PROFILING */

//    unlock();
}

void BGE::InputService::getInputPoints(Space *space, GameObject *gameObj, std::vector<Vector3>& bboxPoints, std::vector<Vector3>& scaledBBoxPoints) {
    if (gameObj && gameObj->isActive() && gameObj->canInteract(space)) {
        auto button = gameObj->getComponent<ButtonComponent>(space);
        auto xform = gameObj->getComponent<TransformComponent>(space);
        BoundingBoxComponent *bbox = nullptr;
        
        if (button) {
            // Buttons need to check their parent because often button's bezels are the ones not visible
            auto parent = gameObj->getParent(space);
            
            if (!parent || (parent->isActive() && parent->canInteract(space))) {
                // Compute collision if exists
                bbox = button->getBoundingBox();
            }
        }
        else {
            auto inputTouch = gameObj->getComponent<InputTouchComponent>(space);
                
            if (inputTouch) {
                bbox = gameObj->getComponent<BoundingBoxComponent>(space);
            }
        }
        
        if (bbox) {
            Matrix4 matrix;
            xform->getWorldMatrix(matrix);
            bbox->computeAABB(matrix);
            // Add points
            bboxPoints.push_back(Vector3{ {bbox->aabbMinX, bbox->aabbMinY, 0}});
            bboxPoints.push_back(Vector3{ {bbox->aabbMaxX, bbox->aabbMinY, 0}});
            bboxPoints.push_back(Vector3{ {bbox->aabbMaxX, bbox->aabbMaxY, 0}});
            bboxPoints.push_back(Vector3{ {bbox->aabbMinX, bbox->aabbMaxY, 0}});
            
            if (xform->useCollisionRectScale()) {
                bbox->computeAABB(matrix, xform->getCollisionRectScale());
                
                // Add points
                scaledBBoxPoints.push_back(Vector3{ {bbox->aabbMinX, bbox->aabbMinY, 0}});
                scaledBBoxPoints.push_back(Vector3{ {bbox->aabbMaxX, bbox->aabbMinY, 0}});
                scaledBBoxPoints.push_back(Vector3{ {bbox->aabbMaxX, bbox->aabbMaxY, 0}});
                scaledBBoxPoints.push_back(Vector3{ {bbox->aabbMinX, bbox->aabbMaxY, 0}});
            }
        }
        
        uint32_t numChildren = xform->getNumChildren();
        for (uint32_t i=0;i<numChildren;i++) {
            auto childXform = xform->childAtIndex(space, i);
            if (childXform->hasGameObject()) {
                auto childObj = childXform->getGameObjectLockless(space);
                
                // TODO: Have some better means of identifying the right child. For now brute force it
                if (childObj) {
                    getInputPoints(space, childObj, bboxPoints, scaledBBoxPoints);
                }
            }
        }
    }
}
