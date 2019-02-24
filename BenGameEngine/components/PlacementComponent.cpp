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
BGE::ComponentTypeId BGE::PlacementComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::PlacementComponent::type_index_ = typeid(BGE::PlacementComponent);

BGE::PlacementComponent::PlacementComponent() : Component() {
}

void BGE::PlacementComponent::setPlacementReference(PlacementReference *placementRef) {
    if (placementRef) {
        setPlacementReference(*placementRef);
    }
}

void BGE::PlacementComponent::setPlacementReference(const PlacementReference &placementRef) {
    width_ = placementRef.width;
    height_ = placementRef.height;
}

void BGE::PlacementComponent::setWidth(float width) {
    width_ = width;
}

void BGE::PlacementComponent::setHeight(float height) {
    height_ = height;
}

void BGE::PlacementComponent::setWidthHeight(Vector2 &wh) {
    width_ = wh.w;
    height_ = wh.h;
}

void BGE::PlacementComponent::addChild(GameObjectHandle handle) {
    auto space = getSpace();
    auto gameObj = getGameObject(space);
    
    if (gameObj) {
        auto xform = gameObj->getComponent<TransformComponent>(space);
        if (xform) {
            auto otherGameObj = space->getGameObject(handle);
            if (otherGameObj) {
                xform->addChild(otherGameObj);
            }
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
}

void BGE::PlacementComponent::addChild(GameObject *object) {
    if (object) {
        auto space = getSpace();
        auto gameObj = getGameObject(space);
        
        if (gameObj) {
            auto xform = gameObj->getComponent<TransformComponent>(space);
            if (xform) {
                xform->addChild(object);
            } else {
                assert(false);
            }
        } else {
            assert(false);
        }
    }
}
