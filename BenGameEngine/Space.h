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
    class AnimationSequenceComponent;
    class Game;
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
        
        void initialize(SpaceHandle handle, std::string name, std::shared_ptr<SpaceService> service);
        void destroy();
        void reset(std::function<void()> callback);
        
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
        void getRootGameObjects(std::vector<GameObject *> &objects);
        void getReverseRootGameObjects(std::vector<GameObject *> &objects);
        
        void getTransforms(std::vector<TransformComponent *> &xforms) const;
        void getRootTransforms(std::vector<TransformComponent *> &xforms) const;
        
        template <typename T, typename... Args> T *createComponent(Args&& ...args) {
            lock();
            
            auto component = componentService_->createComponent<T>(std::forward<Args>(args)...);
            
            if (component) {
                component->setSpaceHandle(componentService_->getSpaceHandle());
            }
            
            unlock();
            
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
            lock();
            
            componentService_->removeComponent(handle);
            
            unlock();
        }
        
        template <typename T> inline void removeAllComponents()  {
            lock();
            
            componentService_->removeAllComponents<T>();
            
            unlock();
        }

        inline bool isActive() const { return active_; }
        inline void setActive(bool active) { active_ = active; }
        
        inline bool isVisible() const { return active_ && visible_; }
        inline void setVisible(bool visible) { visible_ = visible; }

        inline bool isUpdatable() const { return updatable_; }
        inline void setUpdatable(bool updatable) { updatable_ = updatable; }
        
        inline uint32_t getOrder() const { return order_; }
        inline void setOrder(uint32_t order) { order_ = order; }
        
        void linkAll();
        
        // Component management
        GameObject *createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createButton(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createMask(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createSprite(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createText(std::string name, std::string instanceName, ScenePackageHandle handle);
        GameObject *createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle);
        
        GameObject *createFlatRect(std::string instanceName, Vector2 &wh, Color &color);
        GameObject *createSprite(std::string instanceName, TextureHandle texHandle);
        GameObject *createSprite(std::string instanceName, Texture *texture);
        
        void createAutoDisplayObjects(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate *delegate, std::function<void()> callback);

        // Font management
        void createFont(std::string name, uint32_t pxSize, std::function<void(FontHandle handle, std::shared_ptr<Error>)> callback);

        // Texture management
        void createTextureFromFile(std::string name, std::string filename, TextureFormat format, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::function<void(Texture *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromFile(std::string name, std::string filename, std::vector<SubTextureDef> &subTextureDefs, TextureFormat format, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);
        void createTextureAtlasFromBuffer(std::string name, void *buffer, TextureFormat format, uint32_t width, uint32_t height, std::vector<SubTextureDef> subTextureDefs, std::function<void(TextureAtlas *, std::shared_ptr<Error>)> callback);

        TextureHandle getTextureHandle(std::string name) const;
        TextureAtlasHandle getTextureAtlasHandle(std::string name) const;
        Texture *getTexture(std::string name) const;
        TextureAtlas *getTextureAtlas(std::string name) const;

        void removeTexture(TextureHandle handle);
        void removeTextureAtlas(TextureAtlasHandle handle);

        void scenePackageAdded(ScenePackageHandle handle);
        void scenePackageRemoved(ScenePackageHandle handle);
        
        template <typename T>
        void setGameObjectHandle(T *component, GameObjectHandle gameObjHandle) {
            component->setGameObjectHandle(gameObjHandle);
            
            ComponentHandle handle{T::typeId_, component->getRawHandle()};

            setGameObjectHandle(handle, gameObjHandle);
        }
        
        void setGameObjectHandle(ComponentHandle compHandle, GameObjectHandle gameObjHandle);
        
        void setAnimationChannelReference(AnimationChannelComponentHandle channelHandle, const AnimationChannelReference *animChanRef);

    protected:
        SpaceHandle spaceHandle_;
        
    private:
        friend SpaceService;
        friend Game;
        friend AnimationSequenceComponent;
        
        using CreatedGameObjectVector = std::vector<std::pair<ComponentTypeId, GameObjectHandle>>;
        
        std::shared_ptr<SpaceService>       spaceService_;
        std::shared_ptr<GameObjectService>  gameObjectService_;
        std::shared_ptr<ComponentService>   componentService_;
        
        std::vector<ScenePackageHandle>     scenePackages_;
        std::vector<FontHandle>             fonts_;
        std::vector<TextureHandle>          textures_;
        std::vector<TextureAtlasHandle>     textureAtlases_;

        bool        active_;    // Active
        bool        visible_;   // Space can be rendered
        bool        updatable_; // Objects in space can be updated
        
        uint32_t    order_;
        
        void lock();
        void unlock();
        
        void reset_();
        
        static uint32_t handlerBitmaskForSceneObjectCreatedDelegate(SceneObjectCreatedDelegate *delegate);
        
        GameObjectHandle handleForGameObject(GameObject *object) const;
        
        GameObject *createAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createAnimChannel(std::string name, std::string instanceName, const AnimationChannelReference *channelRef, SceneObjectCreatedDelegate *delegate);
        GameObject *createFrameAnimSequence(std::string name, std::string instanceName, ScenePackageHandle handle, SceneObjectCreatedDelegate *delegate);
        GameObject *createButton(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createExternalReference(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createMask(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createSprite(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createText(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        GameObject *createPlacement(std::string name, std::string instanceName, ScenePackageHandle handle, uint32_t pushBitmask, CreatedGameObjectVector *objects);
        
        void createAutoDisplayObjects_(GameObjectHandle rootHandle, ScenePackageHandle packageHandle, SceneObjectCreatedDelegate *delegate, std::function<void()> callback);

        void addCreatedGameObjectsForAnimSequence(GameObject *animSequence, uint32_t pushBitmask, CreatedGameObjectVector *objects) const;
        void addCreatedGameObjectsForButton(GameObject *button, uint32_t pushBitmask, CreatedGameObjectVector *objects) const;

        template <typename T>
        void addCreatedGameObjectsForRenderComponent(GameObject *object, CreatedGameObjectVector *objects) const {
            if (object) {
                objects->push_back(std::make_pair(T::typeId_, handleForGameObject(object)));
            }
        }
        
        void setAnimationSequenceReference(AnimationSequenceComponentHandle animSeqHandle, AnimationSequenceReference *animSeqRef);
        void setAnimationSequenceReference(AnimationSequenceComponentHandle animSeqHandle, const AnimationSequenceReference& animSeqRef);

        void dispatchCreatedHandlers(CreatedGameObjectVector *objects, SceneObjectCreatedDelegate *delegate);
    };
}

#endif /* BGESpace_h */
