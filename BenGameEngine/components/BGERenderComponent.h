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
#include <memory>
#include <vector>
#include "BGEComponent.h"
#include "BGEMathTypes.h"
#include "BGEMaterial.h"

enum class BGERenderComponentAnchor {
    // TODO: add upper left, lower left, upper right, lower right
    Center
};

class BGERenderComponent : public BGEComponent {
public:
    float getLocalWidth() { return localBounds_.w; }
    float getLocalHeight() { return localBounds_.h; }
    float getGlobalWidth() { return globalBounds_.w; }
    float getGlobalHeight() { return globalBounds_.h; }

    void getLocalBounds(BGERect& bounds) { bounds = localBounds_; }
    void getGlobalBounds(BGERect& bounds);
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    BGERenderComponentAnchor getAnchor() const { return anchor_; }
    
    std::weak_ptr<BGEMaterial> getMaterial(uint32_t index=0);
    void setMaterials(std::vector<std::shared_ptr<BGEMaterial>> materials);

protected:
    BGERenderComponent(uint64_t componentId);
    BGERenderComponent(uint64_t componentId, std::string name);
    BGERenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGEGameObject> gameObject);
    
    bool getGlobalBoundsDirty() const { return globalBoundsDirty_; }
    void setGlobalBoundsDirty(bool dirty) { globalBoundsDirty_ = dirty; }
    
    void setLocalBounds(BGERect& bounds) { localBounds_ = bounds; }
    void setGlobalBounds(BGERect& bounds) { globalBounds_ = bounds; }
    
    virtual void materialsUpdated() =0;
    
private:
    friend BGEComponentService;
    
    bool                    enabled_;
    bool                    globalBoundsDirty_;
    BGERect                 localBounds_;   // Relative (local) to render component
    BGERect                 globalBounds_;  // Global. This includes any transforms applied
    BGERenderComponentAnchor   anchor_;
    
    std::vector<std::weak_ptr<BGEMaterial>> materials_;
};

#endif /* BGERenderComponent_h */
