//
//  PolyLineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/1/17.
//  Copyright © 2017 2n Productions. All rights reserved.
//

#include "PolyLineRenderComponent.h"

uint32_t BGE::PolyLineRenderComponent::bitmask_ = Component::InvalidBitmask;
BGE::ComponentTypeId BGE::PolyLineRenderComponent::typeId_ = Component::InvalidTypeId;
std::type_index BGE::PolyLineRenderComponent::type_index_ = typeid(BGE::PolyLineRenderComponent);

BGE::PolyLineRenderComponent::PolyLineRenderComponent() : RenderComponent(), thickness_(1), joint_(Joint::miter), cap_(Cap::both), feather_(false), feathering_(0), noFeatherAtCap_(false), noFeatherAtCore_(false) {
}

const std::vector<BGE::Vector2>& BGE::PolyLineRenderComponent::getPoints() const {
    return points_;
}

void BGE::PolyLineRenderComponent::setPoints(const std::vector<Vector2>& points, bool lineLoop) {
    points_ = points;
    
    // Calculate bounds
}

const std::vector<BGE::Color>& BGE::PolyLineRenderComponent::getColors() const {
    return colors_;
}

void BGE::PolyLineRenderComponent::setColors(const std::vector<Color>& colors) {
    colors_ = colors;
}
