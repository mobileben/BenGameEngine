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
#include "MathTypes.h"
#include "Material.h"

namespace BGE {
    enum class RenderComponentAnchor {
        // TODO: add upper left, lower left, upper right, lower right
        Center
    };
    
    class RenderComponent : public Component {
    public:
        virtual ~RenderComponent();
        
        float getLocalWidth() { return localBounds_.w; }
        float getLocalHeight() { return localBounds_.h; }
        float getGlobalWidth() { return globalBounds_.w; }
        float getGlobalHeight() { return globalBounds_.h; }
        
        void getLocalBounds(Rect& bounds) { bounds = localBounds_; }
        void getGlobalBounds(Rect& bounds);
        bool isEnabled() const { return enabled_; }
        void setEnabled(bool enabled) { enabled_ = enabled; }
        RenderComponentAnchor getAnchor() const { return anchor_; }
        
        MaterialHandle getMaterialHandle(uint32_t index=0);
        Material *getMaterial(uint32_t index=0);
        void setMaterials(std::vector<MaterialHandle> materials);
        
    protected:
        RenderComponent(ObjectId componentId);
        
        bool getGlobalBoundsDirty() const { return globalBoundsDirty_; }
        void setGlobalBoundsDirty(bool dirty) { globalBoundsDirty_ = dirty; }
        
        void setLocalBounds(Rect& bounds) { localBounds_ = bounds; }
        void setGlobalBounds(Rect& bounds) { globalBounds_ = bounds; }
        
        virtual void materialsUpdated() =0;
        
    private:
        friend ComponentService;
        
        bool                    enabled_;
        bool                    globalBoundsDirty_;
        Rect                    localBounds_;   // Relative (local) to render component
        Rect                    globalBounds_;  // Global. This includes any transforms applied
        RenderComponentAnchor   anchor_;
        
        std::vector<MaterialHandle> materialHandles_;
    };
}

#endif /* BGERenderComponent_h */
