//
//  BoundingBoxComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 7/21/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BoundingBoxComponent.h"

uint32_t BGE::BoundingBoxComponent::bitmask_ = Component::InvalidBitmask;
uint32_t BGE::BoundingBoxComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::BoundingBoxComponent::type_index_ = typeid(BGE::BoundingBoxComponent);

BGE::BoundingBoxComponent::BoundingBoxComponent() : Component(), x(0), y(0), width(0), height(0) {
}

void BGE::BoundingBoxComponent::computeAABB(Matrix4 &transform) {
    Vector2 pt;
    Vector2 newPt;
    
    pt.x = x - width/2.0;
    pt.y = y - height/2.0;
    
    Matrix4MultiplyVector2(newPt, transform, pt);
    
    aabbMinX = newPt.x;
    aabbMinY = newPt.y;
    aabbMaxX = newPt.x;
    aabbMaxY = newPt.y;
    
    pt.x += width;

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
    
    pt.y += height;

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

    pt.x -=width;
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
