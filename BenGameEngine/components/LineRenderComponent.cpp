//
//  LineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LineRenderComponent.h"

uint32_t BGE::LineRenderComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::LineRenderComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::LineRenderComponent::type_index_ = typeid(BGE::LineRenderComponent);

BGE::LineRenderComponent::LineRenderComponent() : RenderComponent(), thickness_(1), closedLoop_(false) {
}

const std::vector<BGE::Vector2>& BGE::LineRenderComponent::getPoints() const {
    return points_;
}

void BGE::LineRenderComponent::setPoints(const std::vector<Vector2>& points, bool lineLoop) {
    points_ = points;
    closedLoop_ = lineLoop;
    
    // Calculate bounds
}

