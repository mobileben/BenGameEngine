//
//  TransformComponent.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 3/31/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGETransformComponent_h
#define BGETransformComponent_h

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include "BGEMathTypes.h"
#include "Component.h"

namespace BGE {
    class TransformComponent : public BGE::Component
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<TransformComponent> create(uint64_t componentId);
        static std::shared_ptr<TransformComponent> create(uint64_t componentId, std::string name);

        TransformComponent(struct private_key const& key, uint64_t componentId);
        TransformComponent(struct private_key const& key, uint64_t componentId, std::string name);

        virtual ~TransformComponent() {}
        
        bool isVisible() const { return visible_; }
        void setVisibility(bool visible) { visible_ = visible; }
        
        bool isInteractable() const { return interactable_; }
        void setInteractability(bool interactable) { interactable_ = interactable; }
        
        bool isInteractableWhenHidden() const { return interactableWhenHidden_; }
        void setInteractabilityWhenHidden(bool interactableWhenHidden) { interactableWhenHidden_ = interactableWhenHidden; }
        
        void getBounds(BGERect &bounds) { bounds = bounds_; }
        
        float getX() const { return position_.x; }
        float getY() const { return position_.y; }
        float getZ() const { return z_; }
        
        void setX(float x) { position_.x = x; transformDirty_ = true; }
        void setY(float y) { position_.y = y; transformDirty_ = true; }
        void setZ(float z) { z_ = z; transformDirty_ = true; }
        
        BGEVector2 getPosition() const { return position_; }
        void setPosition(BGEVector2 &position) { position_ = position; transformDirty_ = true; }
        
        float getScaleX() const { return scale_.x; }
        float getScaleY() const { return scale_.y; }
        void setScaleX(float x) { scale_.x = x; transformDirty_ = true; }
        void setScaleY(float y) { scale_.y = y; transformDirty_ = true; }
        
        BGEVector2 getScale() const { return scale_; }
        void setScale(BGEVector2 &scale) { scale_ = scale; transformDirty_ = true; }
        
        float getSkewX() const { return skew_.x; }
        float getSkewY() const { return skew_.y; }
        void setSkewX(float x) { skew_.x = x; transformDirty_ = true; }
        void setSkewY(float y) { skew_.y = y; transformDirty_ = true; }
        
        BGEVector2 getSkew() const { return skew_; }
        void setSkew(BGEVector2 &skew) { skew_ = skew; transformDirty_ = true; }
        
        float getRotation() const { return rotation_; }
        void setRotation(float rotation) { rotation_ = rotation; transformDirty_ = true; }
        
        void getMatrix(BGEMatrix4 &matrix);
        
        std::weak_ptr<TransformComponent> getParent() { return parent_; }
        std::vector<std::shared_ptr<TransformComponent>> getChildren() { return children_; }
        uint32_t getNumChildren() const { return (uint32_t) children_.size(); }
        
        virtual void addChild(std::shared_ptr<TransformComponent> child);
        virtual void removeAllChildren();
        virtual void removeFromParent();
        
        virtual void insertChild(std::shared_ptr<TransformComponent> child, uint32_t index);
        virtual void moveToParent(std::shared_ptr<TransformComponent> parent);
        
        virtual std::shared_ptr<TransformComponent> childAtIndex(uint32_t index);
        virtual std::shared_ptr<TransformComponent> childWithName(std::string name, bool descned=false);
        
        virtual bool hasChild(std::shared_ptr<TransformComponent> child, bool descend=false);
        virtual bool inParentHierarchy(std::shared_ptr<TransformComponent> parent);
        
    protected:
        friend BGE::ComponentService;
        
        TransformComponent(uint64_t componentId);
        TransformComponent(uint64_t componentId, std::string name);
        
        // Identity
        bool            visible_;
        bool            interactable_;
        bool            interactableWhenHidden_;
        
        // Bounds
        BGERect         bounds_;
        
        // Transformation
        BGEVector2      position_;
        float           z_;
        
        BGEVector2      scale_;
        BGEVector2      skew_;
        
        float           rotation_;
        
        BGEMatrix4      matrix_;
        
        bool            transformDirty_; // Indicates that components and matrix are out of sync
        
        // Hierarchy
        std::weak_ptr<TransformComponent> parent_;
        std::vector<std::shared_ptr<TransformComponent>> children_;
        
        // Control
        // TODO: Support?
        float           speed_;
        bool            paused_;
        
    };
}

#endif /* BGETransformComponent_h */
