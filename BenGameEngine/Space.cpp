//
//  Space.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Space.h"

BGE::Space::Space(struct private_key const& key, uint64_t spaceId, std::shared_ptr<BGE::GameObjectService> gameObjectService) : BGE::Object(spaceId), gameObjectService_(gameObjectService) {
}

BGE::Space::Space(struct private_key const& key, uint64_t spaceId, std::string name, std::shared_ptr<BGE::GameObjectService> gameObjectService) : BGE::Object(spaceId, name), gameObjectService_(gameObjectService) {
}

std::shared_ptr<BGE::Space> BGE::Space::create(uint64_t spaceId, std::shared_ptr<BGE::GameObjectService> gameObjectService) {
    return std::make_shared<Space>(private_key{}, spaceId, gameObjectService);
}

std::shared_ptr<BGE::Space> BGE::Space::create(uint64_t spaceId, std::string name, std::shared_ptr<BGE::GameObjectService> gameObjectService) {
    return std::make_shared<Space>(private_key{}, spaceId, name, gameObjectService);
}
