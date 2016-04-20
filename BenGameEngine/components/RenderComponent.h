//
//  RenderComponent.h
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
#include "Component.h"
#include "BGEMathTypes.h"
#include "Material.h"

namespace BGE {
    enum class RenderComponentAnchor {
        // TODO: add upper left, lower left, upper right, lower right
        Center
    };
    
    class RenderComponent : public BGE::Component {
    public:
        float getLocalWidth() { return localBounds_.w; }
        float getLocalHeight() { return localBounds_.h; }
        float getGlobalWidth() { return globalBounds_.w; }
        float getGlobalHeight() { return globalBounds_.h; }
        
        void getLocalBounds(BGERect& bounds) { bounds = localBounds_; }
        void getGlobalBounds(BGERect& bounds);
        bool isEnabled() const { return enabled_; }
        void setEnabled(bool enabled) { enabled_ = enabled; }
        RenderComponentAnchor getAnchor() const { return anchor_; }
        
        std::weak_ptr<BGE::Material> getMaterial(uint32_t index=0);
        void setMaterials(std::vector<std::shared_ptr<BGE::Material>> materials);
        
    protected:
        RenderComponent(uint64_t componentId);
        RenderComponent(uint64_t componentId, std::string name);
        RenderComponent(uint64_t componentId, std::string name, std::shared_ptr<BGE::GameObject> gameObject);
        
        bool getGlobalBoundsDirty() const { return globalBoundsDirty_; }
        void setGlobalBoundsDirty(bool dirty) { globalBoundsDirty_ = dirty; }
        
        void setLocalBounds(BGERect& bounds) { localBounds_ = bounds; }
        void setGlobalBounds(BGERect& bounds) { globalBounds_ = bounds; }
        
        virtual void materialsUpdated() =0;
        
    private:
        friend BGE::ComponentService;
        
        bool                    enabled_;
        bool                    globalBoundsDirty_;
        BGERect                 localBounds_;   // Relative (local) to render component
        BGERect                 globalBounds_;  // Global. This includes any transforms applied
        RenderComponentAnchor   anchor_;
        
        std::vector<std::weak_ptr<BGE::Material>> materials_;
    };
}

#endif /* BGERenderComponent_h */
