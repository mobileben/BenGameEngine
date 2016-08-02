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
#include "MathTypes.h"
#include "Component.h"

namespace BGE {
    class RenderService;
    class RenderServiceOpenGLES2;
    
    class TransformComponent : public Component
    {
    private:
        struct private_key {};
        friend Component;
        static uint32_t         bitmask_;
        static std::type_index  type_index_;
        
    public:
        static std::shared_ptr<TransformComponent> create(ObjectId componentId);

        TransformComponent(struct private_key const& key, ObjectId componentId);
        ~TransformComponent() {}
        
        bool isVisible() const { return visible_; }
        void setVisibility(bool visible) { visible_ = visible; }
        
        void getBounds(Rect &bounds) { bounds = bounds_; }
        
        float getX() const { return position_.x; }
        float getY() const { return position_.y; }
        float getZ() const { return z_; }
        
        void setX(float x) { position_.x = x; transformDirty_ = true; }
        void setY(float y) { position_.y = y; transformDirty_ = true; }
        void setZ(float z) { z_ = z; transformDirty_ = true; }
        
        Vector2 getPosition() const { return position_; }
        void setPosition(Vector2 &position) { position_ = position; transformDirty_ = true; }
        
        float getScaleX() const { return scale_.x; }
        float getScaleY() const { return scale_.y; }
        void setScaleX(float x) { scale_.x = x; transformDirty_ = true; }
        void setScaleY(float y) { scale_.y = y; transformDirty_ = true; }
        
        Vector2 getScale() const { return scale_; }
        void setScale(Vector2 &scale) { scale_ = scale; transformDirty_ = true; }
        
        float getSkewX() const { return skew_.x; }
        float getSkewY() const { return skew_.y; }
        void setSkewX(float x) { skew_.x = x; transformDirty_ = true; }
        void setSkewY(float y) { skew_.y = y; transformDirty_ = true; }
        
        Vector2 getSkew() const { return skew_; }
        void setSkew(Vector2 &skew) { skew_ = skew; transformDirty_ = true; }
        
        float getRotation() const { return rotation_; }
        void setRotation(float rotation) { rotation_ = rotation; transformDirty_ = true; }
        
        // TODO: This should be made so that matrices are exposed. This is to reduce overhead of copies
        void getMatrix(Matrix4 &matrix);
        const float *getMatrixRaw();
        
        void getLocalMatrix(Matrix4 &matrix);
        const float *getLocalMatrixRaw();
        
        std::weak_ptr<TransformComponent> getParent() { return parent_; }
        std::vector<std::shared_ptr<TransformComponent>> getChildren() { return children_; }
        uint32_t getNumChildren() const { return (uint32_t) children_.size(); }
        
        virtual void addChild(std::shared_ptr<TransformComponent> child);
        virtual void removeAllChildren();
        virtual void removeFromParent();
        
        virtual void insertChild(std::shared_ptr<TransformComponent> child, uint32_t index);
        virtual void moveToParent(std::shared_ptr<TransformComponent> parent);
        
        virtual std::shared_ptr<TransformComponent> childAtIndex(uint32_t index);
        
        virtual bool hasChild(std::shared_ptr<TransformComponent> child, bool descend=false);
        virtual bool inParentHierarchy(std::shared_ptr<TransformComponent> parent);
        
    protected:
        friend ComponentService;
        friend RenderService;
        friend RenderServiceOpenGLES2;
        friend GameObject;
        
        // Identity
        bool            visible_;
        
        // Bounds
        Rect            bounds_;
        
        // Transformation
        Vector2         position_;
        float           z_;
        
        Vector2         scale_;
        Vector2         skew_;
        
        float           rotation_;
        
        Matrix4         matrix_;
        Matrix4         localMatrix_;
        
        bool            transformDirty_; // Indicates that components and matrix are out of sync
        
        // Hierarchy
        std::weak_ptr<TransformComponent> parent_;
        std::vector<std::shared_ptr<TransformComponent>> children_;
        
        // Control
        // TODO: Support?
        float           speed_;
        bool            paused_;
        
        void setGameObject(std::shared_ptr<GameObject> gameObject);
        void updateMatrix();
    };
}

#endif /* BGETransformComponent_h */
