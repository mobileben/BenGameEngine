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
    class GameObject;
    
    class TransformComponent : public Component
    {
    public:
        static std::type_index  type_index_;
        static ComponentTypeId  typeId_;
        static uint32_t         bitmask_;
        
        TransformComponent();
        ~TransformComponent() {}
        
        void initialize(HandleBackingType handle, SpaceHandle spaceHandle) final;
        void destroy() final;
        
        bool isVisible() const { return visible_; }
        void setVisibility(bool visible) { visible_ = visible; }
        
        void getBounds(Rect &bounds) { bounds = bounds_; }
        
        float getX() const { return position_.x; }
        float getY() const { return position_.y; }
        float getZ() const { return z_; }
        
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        
        Vector2 getPosition() const { return position_; }
        void setPosition(Vector2 &position);
        void setPosition(Vector2 &&position);
        
        Vector2 getGlobalPosition();
        
        float getScaleX() const { return scale_.x; }
        float getScaleY() const { return scale_.y; }
        void setScaleX(float x);
        void setScaleY(float y);
        
        Vector2 getScale() const { return scale_; }
        void setScale(Vector2 &scale);
        void setScale(Vector2 &&scale);
        
        float getSkewX() const { return skew_.x; }
        float getSkewY() const { return skew_.y; }
        void setSkewX(float x);
        void setSkewY(float y);
        
        Vector2 getSkew() const { return skew_; }
        void setSkew(Vector2 &skew);
        void setSkew(Vector2 &&skew);
        
        void forceDirty();
        
        float getRotationInDegrees() const { return rotation_ * (180.0/M_PI); }
        void setRotationInDegrees(float rotation);

        float getRotationInRadians() const { return rotation_; }
        void setRotationInRadians(float rotation);
        
        // TODO: This should be made so that matrices are exposed. This is to reduce overhead of copies
        void getWorldMatrix(Matrix4 &matrix);
        const float *getWorldMatrixRaw();
        
        void getLocalMatrix(Matrix4 &matrix);
        const float *getLocalMatrixRaw();
        
        TransformComponentHandle getParentHandle() const { return parentHandle_; }
        TransformComponent *getParent() const;
        
        std::vector<TransformComponentHandle> getChildrenHandles() { return childrenHandles_; }
        std::vector<TransformComponentHandle> getOrderedChildrenHandles();
        
        std::vector<TransformComponent *> getChildren();
        std::vector<TransformComponent *> getOrderedChildren();
        
        uint32_t getNumChildren() const { return (uint32_t) childrenHandles_.size(); }
        
        void addChild(TransformComponentHandle handle);
        void addChild(TransformComponent *child);
        void addChild(GameObjectHandle handle);
        void addChild(GameObject *child);

        void removeAllChildren();
        void removeFromParent();
        
        void insertChild(TransformComponentHandle handle, uint32_t index);
        void insertChild(TransformComponent *child, uint32_t index);
        void insertChild(GameObjectHandle handle, uint32_t index);
        void insertChild(GameObject *child, uint32_t index);
        
        void replaceChild(TransformComponentHandle handle, uint32_t index);
        void replaceChild(TransformComponent *child, uint32_t index);
        void replaceChild(GameObjectHandle handle, uint32_t index);
        void replaceChild(GameObject *child, uint32_t index);
        
        void moveToParent(TransformComponentHandle handle);
        void moveToParent(TransformComponent *parentHandle);
        void moveToParent(GameObjectHandle handle);
        void moveToParent(GameObject *parentHandle);
        
        TransformComponent *childAtIndex(uint32_t index);
        TransformComponentHandle childHandleAtIndex(uint32_t index);
        
        bool hasChild(TransformComponentHandle handle, bool descend=false);
        bool hasChild(TransformComponent *child, bool descend=false);
        bool inParentHierarchy(TransformComponentHandle handle);
        bool inParentHierarchy(TransformComponent *parent);
        
        void updateMatrix();
        void updateMatrixAndChildren(bool parentDirty=false);

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
        bool            useSkew_;   // Note this is Flash skew, not affine skew. When skew is in use, rotation is not used
        Vector2         position_;
        float           z_;
        
        Vector2         scale_;
        Vector2         skew_;
        
        // Rotation is in radians
        float           rotation_;
        
        Matrix4         localMatrix_;
        Matrix4         worldMatrix_;
        
        bool            localDirty_;
        bool            worldDirty_;
        
        // Hierarchy
        TransformComponentHandle                parentHandle_;
        std::vector<TransformComponentHandle>   childrenHandles_;
        
        // Control
        // TODO: Support?
        float           speed_;
        bool            paused_;
        
        void setParentHandle(TransformComponentHandle parentHandle);
        void setParent(TransformComponent *parent);
    };
}

#endif /* BGETransformComponent_h */
