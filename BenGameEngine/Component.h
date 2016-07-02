//
//  Component.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/5/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEComponent_h
#define BGEComponent_h

#include <stdio.h>
#include "Object.h"
#include "Handle.h"

namespace BGE {
    class GameObject;
    class ComponentService;
    struct SpaceTag;
    using SpaceHandle = Handle<SpaceTag>;
    class Space;
    
    class Component : public Object
    {
    private:
        struct private_key {};
        
    public:
        static std::shared_ptr<Component> create(ObjectId componentId);
        
        Component(struct private_key const& key, ObjectId componentId);
        virtual ~Component() {}
        
        bool hasGameObject() const { return !gameObject_.expired(); }
        std::weak_ptr<GameObject> getGameObject() const { return gameObject_; }
        Space *getSpace() const;
        SpaceHandle getSpaceHandle() const { return spaceHandle_; }
        
    protected:
        Component() = delete;
        Component(Component const&) = delete;
        Component(ObjectId componentId);

        void setGameObject(std::shared_ptr<GameObject> gameObject) { gameObject_ = gameObject; }
        void setSpaceHandle(SpaceHandle spaceHandle) { spaceHandle_ = spaceHandle; }

        virtual void created() {}
        
    private:
        friend ComponentService;
        friend GameObject;
        friend Space;
        
        std::weak_ptr<GameObject>   gameObject_;
        SpaceHandle                 spaceHandle_;
    };
}

#endif /* BGEComponent_h */
