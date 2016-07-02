//
//  TextComponent.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "TextComponent.h"

std::shared_ptr<BGE::TextComponent> BGE::TextComponent::create(ObjectId componentId) {
    return std::make_shared<TextComponent>(private_key{}, componentId);
}

BGE::TextComponent::TextComponent(struct private_key const& key, ObjectId componentId) : RenderComponent(componentId) {
}

void BGE::TextComponent::materialsUpdated() {
}

