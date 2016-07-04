//
//  LineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LineRenderComponent.h"

uint32_t BGE::LineRenderComponent::bitmask_ = Component::InvalidBitmask;
std::type_index BGE::LineRenderComponent::type_index_ = typeid(BGE::LineRenderComponent);

std::shared_ptr<BGE::LineRenderComponent> BGE::LineRenderComponent::create(ObjectId componentId) {
    return std::make_shared<LineRenderComponent>(private_key{}, componentId);
}

BGE::LineRenderComponent::LineRenderComponent(struct private_key const& key, ObjectId componentId) : RenderComponent(componentId), closedLoop_(false) {
}

const std::vector<BGE::Vector2>& BGE::LineRenderComponent::getPoints() const {
    return points_;
}

void BGE::LineRenderComponent::setPoints(const std::vector<Vector2>& points, bool lineLoop) {
    points_ = points;
    closedLoop_ = lineLoop;
    
    // Calculate bounds
}

