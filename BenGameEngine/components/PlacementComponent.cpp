//
//  PlacementComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/11/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "PlacementComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"

uint32_t BGE::PlacementComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::PlacementComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::PlacementComponent::type_index_ = typeid(BGE::PlacementComponent);

BGE::PlacementComponent::PlacementComponent() : Component() {
}

void BGE::PlacementComponent::setPlacementReference(PlacementReference *placementRef) {
    if (placementRef) {
        setPlacementReference(*placementRef);
    }
}

void BGE::PlacementComponent::setPlacementReference(const PlacementReference &placementRef) {
}

void BGE::PlacementComponent::setWidth(float width) {
}

void BGE::PlacementComponent::setHeight(float height) {
}

void BGE::PlacementComponent::setWidthHeight(Vector2 &wh) {
}

void BGE::PlacementComponent::addChild(GameObjectHandle handle) {
    auto gameObj = getGameObject();
    
    if (gameObj) {
        auto xform = gameObj->getComponent<TransformComponent>();
        
        if (xform) {
            auto space = getSpace();
            
            if (space) {
                auto otherGameObj = space->getGameObject(handle);
                
                if (otherGameObj) {
                    xform->addChild(otherGameObj);
                }
            }
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
}

void BGE::PlacementComponent::addChild(GameObject *object) {
    auto gameObj = getGameObject();
    
    if (gameObj) {
        auto xform = gameObj->getComponent<TransformComponent>();
        
        if (xform) {
            if (object) {
                xform->addChild(object);
            }
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
}
