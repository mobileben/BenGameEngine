//
//  TextComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextComponent.h"

std::shared_ptr<BGE::TextComponent> BGE::TextComponent::create(uint64_t componentId) {
    return std::make_shared<TextComponent>(private_key{}, componentId);
}

std::shared_ptr<BGE::TextComponent> BGE::TextComponent::create(uint64_t componentId, std::string name) {
    return std::make_shared<TextComponent>(private_key{}, componentId, name);
}

BGE::TextComponent::TextComponent(struct private_key const& key, uint64_t componentId) : BGE::RenderComponent(componentId) {
}

BGE::TextComponent::TextComponent(struct private_key const& key, uint64_t componentId, std::string name) : BGE::RenderComponent(componentId, name) {
}

BGE::TextComponent::TextComponent(uint32_t componentId) : BGE::RenderComponent(componentId) {
}

BGE::TextComponent::TextComponent(uint32_t componentId, std::string name) : BGE::RenderComponent(componentId, name) {
}

void BGE::TextComponent::materialsUpdated() {
}

