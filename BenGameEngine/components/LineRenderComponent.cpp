//
//  LineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "LineRenderComponent.h"

BGE::LineRenderComponent::LineRenderComponent(uint32_t componentId) : BGE::RenderComponent(componentId), closedLoop_(false) {
}

BGE::LineRenderComponent::LineRenderComponent(uint32_t componentId, std::string name) : BGE::RenderComponent(componentId, name), closedLoop_(false) {
}

BGE::LineRenderComponent::LineRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject) : BGE::RenderComponent(componentId, name, gameObject), closedLoop_(false) {
}

const std::vector<BGEVector2>& BGE::LineRenderComponent::getPoints() const {
    return points_;
}

void BGE::LineRenderComponent::setPoints(const std::vector<BGEVector2>& points, bool lineLoop) {
    points_ = points;
    closedLoop_ = lineLoop;
    
    // Calculate bounds
}

