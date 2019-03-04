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

BGE::PolyLineRenderComponent::PolyLineRenderComponent() : RenderComponent(), thickness_(1), joint_(Joint::miter), cap_(Cap::both), feather_(false), feathering_(0), noFeatherAtCap_(false), noFeatherAtCore_(false), dirty_(false) {
}

void BGE::PolyLineRenderComponent::destroy() {
    dirty_ = false;
    
    // Component::destroy last
    Component::destroy();
}

void BGE::PolyLineRenderComponent::destroyFast() {
    dirty_ = false;
    
    // Component::destroyFast last
    Component::destroyFast();
}

const std::vector<BGE::Vector2>& BGE::PolyLineRenderComponent::getPoints() const {
    return points_;
}

void BGE::PolyLineRenderComponent::setPoints(const std::vector<Vector2>& points, __attribute__ ((unused)) bool lineLoop) {
    points_ = points;
    dirty_ = true;
    // Calculate bounds
}

const std::vector<BGE::Color>& BGE::PolyLineRenderComponent::getColors() const {
    return colors_;
}

void BGE::PolyLineRenderComponent::setColors(const std::vector<Color>& colors) {
    colors_ = colors;
    dirty_ = true;
}

void BGE::PolyLineRenderComponent::generateVertexArrayHolder(VASEr::LineContext& context, std::vector<VASEr::Vec2>& scratchPoints, std::vector<VASEr::Color>& scratchColors) {
    const auto& points = points_;
    const auto& colors = colors_;
    
    // Convert our data to what VASEr needs
    auto numColors = colors.size();
    
    scratchPoints.resize(points.size());
    scratchColors.resize(points.size());
    
    VASEr::Vec2 *vPoints = &scratchPoints[0];
    VASEr::Color *vColors = &scratchColors[0];
    
    auto numPoints = points.size();
    assert(numPoints == numColors || numColors == 1);
    
    for (size_t i=0;i<numPoints;++i) {
        auto colorIndex = i;
        vPoints[i].x = points[i].x;
        vPoints[i].y = points[i].y;
        
        if (numColors == 1) {
            colorIndex = 0;
        }
        vColors[i].r = colors[colorIndex].r;
        vColors[i].g = colors[colorIndex].g;
        vColors[i].b = colors[colorIndex].b;
        vColors[i].a = colors[colorIndex].a;
    }
    
    VASEr::polyline_opt opt{};
    
    switch (getJoint()) {
        case PolyLineRenderComponent::Joint::bevel:
            opt.joint = VASEr::PLJ_bevel;
            break;
        case PolyLineRenderComponent::Joint::round:
            opt.joint = VASEr::PLJ_round;
            break;
        default:
            opt.joint = VASEr::PLJ_miter;
            break;
    }
    
    switch (getCap()) {
        case PolyLineRenderComponent::Cap::butt:
            opt.cap = VASEr::PLC_butt;
            break;
        case PolyLineRenderComponent::Cap::round:
            opt.cap = VASEr::PLC_round;
            break;
        case PolyLineRenderComponent::Cap::square:
            opt.cap = VASEr::PLC_square;
            break;
        case PolyLineRenderComponent::Cap::rect:
            opt.cap = VASEr::PLC_rect;
            break;
        case PolyLineRenderComponent::Cap::both:
            opt.cap = VASEr::PLC_both;
            break;
        case PolyLineRenderComponent::Cap::first:
            opt.cap = VASEr::PLC_first;
            break;
        case PolyLineRenderComponent::Cap::last:
            opt.cap = VASEr::PLC_last;
            break;
        case PolyLineRenderComponent::Cap::none:
            opt.cap = VASEr::PLC_none;
            break;
    }

    opt.tess = nullptr;
    opt.feather = isFeather();
    opt.feathering = getFeathering();
    opt.no_feather_at_cap = getNoFeatherAtCap();
    opt.no_feather_at_core = getNoFeatherAtCore();

    VASEr::polyline(vPoints, vColors, getThickness(), static_cast<int>(points.size()), &opt, &context);

    // Everything has been built, so we no longer need to tag us as dirty
    dirty_ = false;
}
