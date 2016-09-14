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

        ~Space() {}
        
        void initialize(SpaceHandle handle, std::string name);
        void destroy();
        void reset();
        
        void outputResourceBreakdown(uint32_t numTabs) const;
        void outputMemoryBreakdown(uint32_t numTabs) const;
        
        inline SpaceHandle getHandle() const { return spaceHandle_; }
        
        GameObject *createGameObject(std::string name = "");
        
        GameObjectHandle getGameObjectHandle(ObjectId objId) const;
        GameObjectHandle getGameObjectHandle(std::string name) const;
        
        GameObject *getGameObject(ObjectId objId) const;
        GameObject *getGameObject(std::string name) const;
        GameObject *getGameObject(GameObjectHandle handle) const;
        
        void removeGameObject(GameObjectHandle handle);
        void removeGameObject(GameObject *object);
        
        const std::vector<GameObjectHandle>& getGameObjects() const;
        
        template <typename T, typename... Args> T *createComponent(Args&& ...args) {
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            return component;
        }
        
        template <typename T> inline T *getComponent(Handle<T> handle) const {
            return componentService_->getComponent<T>(handle);
        }
        
        template <typename T> inline T *getComponent(ComponentHandle handle) const {
            return getComponent<T>(handle.handle);
        }
        
        template <typename T> inline T *getComponent(HandleBackingType handle) const {
            return componentService_->getComponent<T>(handle);
        }

        template <typename T> inline void getComponents(std::vector<T *> &components) const {
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
        
        void linkAll();
        
        GameObject *createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createAnimChannel(std::string name, std::string instanceName, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate *delegate);
        GameObject *createFrameAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createButton(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createMask(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createSprite(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createText(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle);
        
        void createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate *delegate);

        void createFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);

        void scenePackageAdded(ScenePackageHandle handle);
        void scenePackageRemoved(ScenePackageHandle handle);
        
    protected:
        SpaceHandle spaceHandle_;
        
    private:
        friend SpaceService;
        
        using CreatedGameObjectVector = std::vector<std::pair<uint32_t, GameObjectHandle>>;
        
        std::shared_ptr<GameObjectService>  gameObjectService_;
        std::shared_ptr<ComponentService>   componentService_;
        
        std::vector<ScenePackageHandle>     scenePackages_;
        std::vector<FontHandle>             fonts_;
        std::vector<TextureHandle>          textures_;
        std::vector<TextureAtlasHandle>     textureAtlases_;
        
        bool        visible_;
        uint32_t    order_;
        bool        updatable_;
        
        static uint32_t handlerBitmaskForSceneObjectCreatedDelegate(SceneObjectCreatedDelegate *delegate);
        
        GameObjectHandle handleForGameObject(GameObject *object) const;
        
        GameObject *createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createButton(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createMask(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createSprite(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createText(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        
        void addCreatedGameObjectsForAnimSequence(GameObject *animSequence, uint32_t pushBitmask, CreatedGameObjectVector *objects) const;
        void addCreatedGameObjectsForButton(GameObject *button, uint32_t pushBitmask, CreatedGameObjectVector *objects) const;

        template <typename T>
        void addCreatedGameObjectsForRenderComponent(GameObject *object, CreatedGameObjectVector *objects) const {
            if (object) {
                objects->push_back(std::make_pair(T::typeId_, handleForGameObject(object)));
            }
        }
        
        void dispatchCreatedHandlers(CreatedGameObjectVector *objects, SceneObjectCreatedDelegate *delegate);
    };
}

#endif /* BGESpace_h */
