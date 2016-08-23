///Users/ben/Development/BenGameEngine/BenGameEngine/Space.h
//  Space.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGESpace_h
#define BGESpace_h

#include <stdio.h>
#include <memory>
#include <vector>
#include "NamedObject.h"
#include "ComponentService.h"
#include "ScenePackage.h"
#include "Handle.h"

namespace BGE {
    class GameObject;
    class GameObjectService;
    class SpaceService;
    
    class Space : public NamedObject
    {
    public:
        Space();
        Space(ObjectId spaceId);
        Space(ObjectId spaceId, std::string name);

        virtual ~Space() {}
        
        void initialize(SpaceHandle handle, std::string name);

        inline SpaceHandle getHandle() const { return spaceHandle_; }
        
        GameObject *createGameObject(std::string name = "");
        GameObjectHandle getGameObjectHandle(ObjectId objId);
        GameObjectHandle getGameObjectHandle(std::string name);
        GameObject *getGameObject(ObjectId objId);
        GameObject *getGameObject(std::string name);
        GameObject *getGameObject(GameObjectHandle handle);
        void removeGameObject(GameObjectHandle handle);
        const std::vector<GameObjectHandle>& getGameObjects() const;
        
        template <typename T, typename... Args> T *createComponent(Args&& ...args) {
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            return component;
        }
        
        template <typename T> inline T *getComponent(ComponentHandle handle) {
            return getComponent<T>(handle.handle);
        }
        
        template <typename T> inline T *getComponent(HandleBackingType handle) {
            return componentService_->getComponent<T>(handle);
        }

        template <typename T> inline void getComponents(std::vector<T *> &components) {
            componentService_->getComponents<T>(components);
        }
        
        inline void removeComponent(ComponentHandle handle) {
            componentService_->removeComponent(handle);
        }
        
        template <typename T> inline void removeAllComponents()  {
            componentService_->removeAllComponents<T>();
        }

        inline bool isVisible() const { return visible_; }
        inline void setVisible(bool visible) { visible_ = visible; }
        inline uint32_t getOrder() const { return order_; }
        inline void setOrder(uint32_t order) { order_ = order; }
        inline bool isUpdatable() const { return updatable_; }
        inline void setUpdatable(bool updatable) { updatable_ = updatable; }
        
        GameObject *createAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createAnimChannel(std::string name, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createFrameAnimSequence(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createButton(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createExternalReference(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createMask(std::string name, ScenePackageHandle handle = ScenePackageHandle(), SceneObjectCreatedDelegate delegate = SceneObjectCreatedDelegate());
        GameObject *createSprite(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        GameObject *createText(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        GameObject *createPlacement(std::string name, ScenePackageHandle handle = ScenePackageHandle());
        
        void createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate delegate);

        void createFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);

    protected:
        SpaceHandle spaceHandle_;
        
    private:
        friend SpaceService;
        
        std::shared_ptr<GameObjectService> gameObjectService_;
        std::shared_ptr<ComponentService> componentService_;
        
        std::vector<FontHandle>             fonts_;
        std::vector<TextureHandle>          textures_;
        std::vector<TextureAtlasHandle>     textureAtlases_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        // TODO:
    };
}

#endif /* BGESpace_h */
