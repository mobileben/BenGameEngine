//
//  BGERenderComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGERenderComponent_h
#define BGERenderComponent_h

#include <stdio.h>
#include "BGEComponent.h"
#include "BGEMathTypes.h"
#include "BGEMaterial.h"

class BGERenderComponent : public BGEComponent {
public:
    BGERenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject);
    
    void getBounds(BGERect& bounds);
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    std::weak_ptr<BGEMaterial> getMaterial();
    void setMaterial(std::shared_ptr<BGEMaterial> material);
    
private:
    bool    enabled_;
    BGERect bounds_;
    
    std::weak_ptr<BGEMaterial> material_;
};

#endif /* BGERenderComponent_h */
