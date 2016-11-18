//
//  BoundingBoxComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BoundingBoxComponent.h"

uint32_t BGE::BoundingBoxComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::BoundingBoxComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::BoundingBoxComponent::type_index_ = typeid(BGE::BoundingBoxComponent);

BGE::BoundingBoxComponent::BoundingBoxComponent() : Component(), x(0), y(0), width(0), height(0), aabbMinX(0), aabbMinY(0), aabbMaxX(0), aabbMaxY(0) {
}

void BGE::BoundingBoxComponent::initialize(HandleBackingType handle, SpaceHandle spaceHandle) {
    Component::initialize(handle, spaceHandle);
    
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    
    aabbMinX = 0;
    aabbMinY = 0;
    aabbMaxX = 0;
    aabbMaxY = 0;
}

void BGE::BoundingBoxComponent::destroy() {
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    
    aabbMinX = 0;
    aabbMinY = 0;
    aabbMaxX = 0;
    aabbMaxY = 0;

    // Component::destroy last
    Component::destroy();
}

void BGE::BoundingBoxComponent::computeAABB(Matrix4 &transform) {
    computeAABB(transform, width, height);
}

void BGE::BoundingBoxComponent::computeAABB(Matrix4 &transform, float w, float h) {
    Vector2 pt;
    Vector2 newPt;
    
    pt.x = x - w/2.0;
    pt.y = y - h/2.0;
    
    Matrix4MultiplyVector2(newPt, transform, pt);
    
    aabbMinX = newPt.x;
    aabbMinY = newPt.y;
    aabbMaxX = newPt.x;
    aabbMaxY = newPt.y;
    
    pt.x += w;
    
    Matrix4MultiplyVector2(newPt, transform, pt);
    
    if (newPt.x > aabbMaxX) {
        aabbMaxX = newPt.x;
    } else if (newPt.x < aabbMinX) {
        aabbMinX = newPt.x;
    }
    
    if (newPt.y > aabbMaxY) {
        aabbMaxY = newPt.y;
    } else if (newPt.y < aabbMinY) {
        aabbMinY = newPt.y;
    }
    
    pt.y += h;
    
    Matrix4MultiplyVector2(newPt, transform, pt);
    
    if (newPt.x > aabbMaxX) {
        aabbMaxX = newPt.x;
    } else if (newPt.x < aabbMinX) {
        aabbMinX = newPt.x;
    }
    
    if (newPt.y > aabbMaxY) {
        aabbMaxY = newPt.y;
    } else if (newPt.y < aabbMinY) {
        aabbMinY = newPt.y;
    }
    
    pt.x -=w;
    Matrix4MultiplyVector2(newPt, transform, pt);
    
    if (newPt.x > aabbMaxX) {
        aabbMaxX = newPt.x;
    } else if (newPt.x < aabbMinX) {
        aabbMinX = newPt.x;
    }
    
    if (newPt.y > aabbMaxY) {
        aabbMaxY = newPt.y;
    } else if (newPt.y < aabbMinY) {
        aabbMinY = newPt.y;
    }
}

void BGE::BoundingBoxComponent::computeAABB(Matrix4 &transform, Vector2 scale) {
    computeAABB(transform, width * scale.x, height * scale.y);
}

