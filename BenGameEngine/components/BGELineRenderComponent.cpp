//
//  BGELineRenderComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/14/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGELineRenderComponent.h"

BGELineRenderComponent::BGELineRenderComponent(uint32_t componentId) : BGERenderComponent(componentId), closedLoop_(false) {
}

BGELineRenderComponent::BGELineRenderComponent(uint32_t componentId, std::string name) : BGERenderComponent(componentId, name), closedLoop_(false) {
}

BGELineRenderComponent::BGELineRenderComponent(uint32_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject) : BGERenderComponent(componentId, name, gameObject), closedLoop_(false) {
}

const std::vector<BGEVector2>& BGELineRenderComponent::getPoints() const {
    return points_;
}

void BGELineRenderComponent::setPoints(const std::vector<BGEVector2>& points, bool lineLoop) {
    points_ = points;
    closedLoop_ = lineLoop;
    
    // Calculate bounds
}

