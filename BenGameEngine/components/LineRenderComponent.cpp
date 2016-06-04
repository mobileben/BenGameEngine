//
//  LineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LineRenderComponent.h"

std::shared_ptr<BGE::LineRenderComponent> BGE::LineRenderComponent::create(uint64_t componentId) {
    return std::make_shared<LineRenderComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::LineRenderComponent> BGE::LineRenderComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<LineRenderComponent>(private_key{}, componentId, name);
}

BGE::LineRenderComponent::LineRenderComponent(struct private_key const& key, uint64_t componentId) : BGE::RenderComponent(componentId), closedLoop_(false) {
}

BGE::LineRenderComponent::LineRenderComponent(struct private_key const& key, uint64_t componentId, std::string name) : BGE::RenderComponent(componentId, name), closedLoop_(false) {
}


BGE::LineRenderComponent::LineRenderComponent(uint32_t componentId) : BGE::RenderComponent(componentId), closedLoop_(false) {
}

BGE::LineRenderComponent::LineRenderComponent(uint32_t componentId, std::string name) : BGE::RenderComponent(componentId, name), closedLoop_(false) {
}

const std::vector<BGE::Vector2>& BGE::LineRenderComponent::getPoints() const {
    return points_;
}

void BGE::LineRenderComponent::setPoints(const std::vector<Vector2>& points, bool lineLoop) {
    points_ = points;
    closedLoop_ = lineLoop;
    
    // Calculate bounds
}

