//
//  ComponentService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ComponentService.h"
#include "AnimationChannelComponent.h"
#include "AnimationSequenceComponent.h"
#include "AnimatorComponent.h"
#include "ChannelFrameAnimatorComponent.h"
#include "ColorMatrixComponent.h"
#include "ColorTransformComponent.h"
#include "FlatRectRenderComponent.h"
#include "FrameAnimatorComponent.h"
#include "LineRenderComponent.h"
#include "LogicComponent.h"
#include "PolyLineRenderComponent.h"
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "ButtonComponent.h"
#include "MaskComponent.h"
#include "TextureMaskComponent.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"
#include "PlacementComponent.h"
#include "Game.h"

#include <algorithm>

bool BGE::ComponentService::componentsRegistered_ = false;
std::vector<void *> BGE::ComponentService::componentHandleServices_;
std::vector<std::function<bool(BGE::HandleBackingType)>> BGE::ComponentService::componentHandleServiceIsHandleBackingNull_;

void BGE::ComponentService::registerComponents() {
    if (!ComponentService::componentsRegistered_) {
        registerComponent<TransformComponent>(40960, HandleServiceNoMaxLimit);
        registerComponent<BoundingBoxComponent>(20480, HandleServiceNoMaxLimit);
        registerComponent<AnimationChannelComponent>(40960, HandleServiceNoMaxLimit);
        registerComponent<AnimationSequenceComponent>(8192, HandleServiceNoMaxLimit);
        registerComponent<AnimatorComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<ChannelFrameAnimatorComponent>(40960, HandleServiceNoMaxLimit);
        registerComponent<ColorMatrixComponent>(3072, HandleServiceNoMaxLimit);
        registerComponent<ColorTransformComponent>(2048, HandleServiceNoMaxLimit);
        registerComponent<FrameAnimatorComponent>(7680, HandleServiceNoMaxLimit);
        registerComponent<SpriteRenderComponent>(20480, HandleServiceNoMaxLimit);
        registerComponent<LogicComponent>(128, HandleServiceNoMaxLimit);
        registerComponent<TextComponent>(512, HandleServiceNoMaxLimit);
        registerComponent<PlacementComponent>(384, HandleServiceNoMaxLimit);
        registerComponent<ButtonComponent>(256, HandleServiceNoMaxLimit);
        registerComponent<InputTouchComponent>(64, HandleServiceNoMaxLimit);
        registerComponent<PolyLineRenderComponent>(2048, HandleServiceNoMaxLimit);
        registerComponent<LineRenderComponent>(128, HandleServiceNoMaxLimit);
        registerComponent<FlatRectRenderComponent>(128, HandleServiceNoMaxLimit);
        registerComponent<MaskComponent>(128, HandleServiceNoMaxLimit);
        registerComponent<TextureMaskComponent>(256, HandleServiceNoMaxLimit);
    }
    
    componentsRegistered_ = true;
}

BGE::ComponentService::ComponentService() : Service() {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
    }
    
    // Create our componentHandle vectors
    for (auto it=componentHandleServices_.begin();it!=componentHandleServices_.end();++it) {
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

void BGE::ComponentService::garbageCollect() {
    garbageCollectComponent<TransformComponent>();
    garbageCollectComponent<BoundingBoxComponent>();
    garbageCollectComponent<AnimationChannelComponent>();
    garbageCollectComponent<AnimationSequenceComponent>();
    garbageCollectComponent<AnimatorComponent>();
    garbageCollectComponent<ChannelFrameAnimatorComponent>();
    garbageCollectComponent<ColorMatrixComponent>();
    garbageCollectComponent<ColorTransformComponent>();
    garbageCollectComponent<FrameAnimatorComponent>();
    garbageCollectComponent<SpriteRenderComponent>();
    garbageCollectComponent<LogicComponent>();
    garbageCollectComponent<TextComponent>();
    garbageCollectComponent<PlacementComponent>();
    garbageCollectComponent<ButtonComponent>();
    garbageCollectComponent<InputTouchComponent>();
    garbageCollectComponent<PolyLineRenderComponent>();
    garbageCollectComponent<LineRenderComponent>();
    garbageCollectComponent<FlatRectRenderComponent>();
    garbageCollectComponent<MaskComponent>();
    garbageCollectComponent<TextureMaskComponent>();
}

BGE::GameObject *BGE::ComponentService::getComponentGameObject(Component *component, GameObjectHandle gameObjHandle) const {
    return component->getSpace()->getGameObject(gameObjHandle);
}

void BGE::ComponentService::removeComponent(ComponentHandle handle) {
    auto typeId = handle.typeId;
    auto &handles = componentHandles_[typeId];
    auto size = handles.size();

    if (!size) {
        return;
    }

    lock();
    auto isHandleBackingNull = componentHandleServiceIsHandleBackingNull_[typeId];
    if (!isHandleBackingNull(handle.handle)) {
        for (size_t i=0;i<size;++i) {
            auto& h = handles[i];
            if (h.handle == handle.handle) {
                if (typeId == TransformComponent::typeId_) {
                    releaseComponentHandle<TransformComponent>(handle);
                } else if (typeId == BoundingBoxComponent::typeId_) {
                    releaseComponentHandle<BoundingBoxComponent>(handle);
                } else if (typeId == AnimationChannelComponent::typeId_) {
                    releaseComponentHandle<AnimationChannelComponent>(handle);
                } else if (typeId == AnimationSequenceComponent::typeId_) {
                    releaseComponentHandle<AnimationSequenceComponent>(handle);
                } else if (typeId == AnimatorComponent::typeId_) {
                    releaseComponentHandle<AnimatorComponent>(handle);
                } else if (typeId == ChannelFrameAnimatorComponent::typeId_) {
                    releaseComponentHandle<ChannelFrameAnimatorComponent>(handle);
                } else if (typeId == ColorMatrixComponent::typeId_) {
                    releaseComponentHandle<ColorMatrixComponent>(handle);
                } else if (typeId == ColorTransformComponent::typeId_) {
                    releaseComponentHandle<ColorTransformComponent>(handle);
                } else if (typeId == FrameAnimatorComponent::typeId_) {
                    releaseComponentHandle<FrameAnimatorComponent>(handle);
                } else if (typeId == SpriteRenderComponent::typeId_) {
                    releaseComponentHandle<SpriteRenderComponent>(handle);
                } else if (typeId == LogicComponent::typeId_) {
                    releaseComponentHandle<LogicComponent>(handle);
                } else if (typeId == TextComponent::typeId_) {
                    RenderStringCacheCommandData data(getSpaceHandle(), TextComponentHandle(handle.handle));
                    Game::getInstance()->getRenderService()->queueDestroyStringCacheEntry(data);
                    releaseComponentHandle<TextComponent>(handle);
                } else if (typeId == PlacementComponent::typeId_) {
                    releaseComponentHandle<PlacementComponent>(handle);
                } else if (typeId == ButtonComponent::typeId_) {
                    releaseComponentHandle<ButtonComponent>(handle);
                } else if (typeId == InputTouchComponent::typeId_) {
                    releaseComponentHandle<InputTouchComponent>(handle);
                } else if (typeId == PolyLineRenderComponent::typeId_) {
                    RenderPolyLineCacheCommandData data(getSpaceHandle(), PolyLineRenderComponentHandle(handle.handle));
                    Game::getInstance()->getRenderService()->queueDestroyPolyLineCacheEntry(data);
                    releaseComponentHandle<PolyLineRenderComponent>(handle);
                } else if (typeId == LineRenderComponent::typeId_) {
                    releaseComponentHandle<LineRenderComponent>(handle);
                } else if (typeId == FlatRectRenderComponent::typeId_) {
                    releaseComponentHandle<FlatRectRenderComponent>(handle);
                } else if (typeId == MaskComponent::typeId_) {
                    releaseComponentHandle<MaskComponent>(handle);
                } else if (typeId == TextureMaskComponent::typeId_) {
                    releaseComponentHandle<TextureMaskComponent>(handle);
                } else {
                    assert(false);
                }

                if (size > 1) {
#ifdef USE_ERASE
                    handles.erase(h);
#else
                    std::swap(handles[i], handles.back());
                    handles.pop_back();
#endif
                }

                unlock();
                return;
            }
        }
    }
    unlock();
}

void BGE::ComponentService::removeAllComponents() {
    lock();
    auto renderService = Game::getInstance()->getRenderService();
    auto size = componentHandles_.size();
    for (ComponentTypeId typeId=0;typeId<size;++typeId) {
        auto& handles = componentHandles_[typeId];
        auto isHandleBackingNull = componentHandleServiceIsHandleBackingNull_[typeId];
        for (auto const &handle : handles) {
            if (!isHandleBackingNull(handle.handle)) {
                if (typeId == TransformComponent::typeId_) {
                    releaseFastComponentHandle<TransformComponent>(handle);
                } else if (typeId == BoundingBoxComponent::typeId_) {
                    releaseFastComponentHandle<BoundingBoxComponent>(handle);
                } else if (typeId == AnimationChannelComponent::typeId_) {
                    releaseFastComponentHandle<AnimationChannelComponent>(handle);
                } else if (typeId == AnimationSequenceComponent::typeId_) {
                    releaseFastComponentHandle<AnimationSequenceComponent>(handle);
                } else if (typeId == AnimatorComponent::typeId_) {
                    releaseFastComponentHandle<AnimatorComponent>(handle);
                } else if (typeId == ChannelFrameAnimatorComponent::typeId_) {
                    releaseFastComponentHandle<ChannelFrameAnimatorComponent>(handle);
                } else if (typeId == ColorMatrixComponent::typeId_) {
                    releaseFastComponentHandle<ColorMatrixComponent>(handle);
                } else if (typeId == ColorTransformComponent::typeId_) {
                    releaseFastComponentHandle<ColorTransformComponent>(handle);
                } else if (typeId == FrameAnimatorComponent::typeId_) {
                    releaseFastComponentHandle<FrameAnimatorComponent>(handle);
                } else if (typeId == SpriteRenderComponent::typeId_) {
                    releaseFastComponentHandle<SpriteRenderComponent>(handle);
                } else if (typeId == LogicComponent::typeId_) {
                    releaseFastComponentHandle<LogicComponent>(handle);
                } else if (typeId == TextComponent::typeId_) {
                    RenderStringCacheCommandData data(getSpaceHandle(), TextComponentHandle(handle.handle));
                    renderService->queueDestroyStringCacheEntry(data);
                    releaseFastComponentHandle<TextComponent>(handle);
                } else if (typeId == PlacementComponent::typeId_) {
                    releaseFastComponentHandle<PlacementComponent>(handle);
                } else if (typeId == ButtonComponent::typeId_) {
                    releaseFastComponentHandle<ButtonComponent>(handle);
                } else if (typeId == InputTouchComponent::typeId_) {
                    releaseFastComponentHandle<InputTouchComponent>(handle);
                } else if (typeId == PolyLineRenderComponent::typeId_) {
                    RenderPolyLineCacheCommandData data(getSpaceHandle(), PolyLineRenderComponentHandle(handle.handle));
                    renderService->queueDestroyPolyLineCacheEntry(data);
                    releaseFastComponentHandle<PolyLineRenderComponent>(handle);
                } else if (typeId == LineRenderComponent::typeId_) {
                    releaseFastComponentHandle<LineRenderComponent>(handle);
                } else if (typeId == FlatRectRenderComponent::typeId_) {
                    releaseFastComponentHandle<FlatRectRenderComponent>(handle);
                } else if (typeId == MaskComponent::typeId_) {
                    releaseFastComponentHandle<MaskComponent>(handle);
                } else if (typeId == TextureMaskComponent::typeId_) {
                    releaseFastComponentHandle<TextureMaskComponent>(handle);
                } else {
                    assert(false);
                }
            }
        }
        handles.clear();
    }
    unlock();
}

uint32_t BGE::ComponentService::totalNumComponents() const {
    uint32_t total = 0;
    
    total += numComponents<TransformComponent>();
    total += numComponents<BoundingBoxComponent>();
    total += numComponents<AnimationChannelComponent>();
    total += numComponents<AnimatorComponent>();
    total += numComponents<AnimationSequenceComponent>();
    total += numComponents<ChannelFrameAnimatorComponent>();
    total += numComponents<ColorMatrixComponent>();
    total += numComponents<ColorTransformComponent>();
    total += numComponents<FrameAnimatorComponent>();
    total += numComponents<SpriteRenderComponent>();
    total += numComponents<LogicComponent>();
    total += numComponents<TextComponent>();
    total += numComponents<PlacementComponent>();
    total += numComponents<ButtonComponent>();
    total += numComponents<InputTouchComponent>();
    total += numComponents<PolyLineRenderComponent>();
    total += numComponents<LineRenderComponent>();
    total += numComponents<FlatRectRenderComponent>();
    total += numComponents<MaskComponent>();
    total += numComponents<TextureMaskComponent>();
    
    return total;
}

uint32_t BGE::ComponentService::totalNumUsedHandles() const {
    return totalNumComponents();
}

uint32_t BGE::ComponentService::totalMaxHandles() const {
    uint32_t total = 0;
    
    total += maxComponents<TransformComponent>();
    total += maxComponents<BoundingBoxComponent>();
    total += maxComponents<AnimationChannelComponent>();
    total += maxComponents<AnimatorComponent>();
    total += maxComponents<AnimationSequenceComponent>();
    total += maxComponents<ChannelFrameAnimatorComponent>();
    total += maxComponents<ColorMatrixComponent>();
    total += maxComponents<ColorTransformComponent>();
    total += maxComponents<FrameAnimatorComponent>();
    total += maxComponents<SpriteRenderComponent>();
    total += maxComponents<LogicComponent>();
    total += maxComponents<TextComponent>();
    total += maxComponents<PlacementComponent>();
    total += maxComponents<ButtonComponent>();
    total += maxComponents<InputTouchComponent>();
    total += maxComponents<PolyLineRenderComponent>();
    total += maxComponents<LineRenderComponent>();
    total += maxComponents<FlatRectRenderComponent>();
    total += maxComponents<MaskComponent>();
    total += maxComponents<TextureMaskComponent>();
    
    return total;
}

uint32_t BGE::ComponentService::totalMaxHandlesAllocated() const {
    uint32_t total = 0;
    
    total += maxHandlesAllocated<TransformComponent>();
    total += maxHandlesAllocated<BoundingBoxComponent>();
    total += maxHandlesAllocated<AnimationChannelComponent>();
    total += maxHandlesAllocated<AnimatorComponent>();
    total += maxHandlesAllocated<AnimationSequenceComponent>();
    total += maxHandlesAllocated<ChannelFrameAnimatorComponent>();
    total += maxHandlesAllocated<ColorMatrixComponent>();
    total += maxHandlesAllocated<ColorTransformComponent>();
    total += maxHandlesAllocated<FrameAnimatorComponent>();
    total += maxHandlesAllocated<SpriteRenderComponent>();
    total += maxHandlesAllocated<LogicComponent>();
    total += maxHandlesAllocated<TextComponent>();
    total += maxHandlesAllocated<PlacementComponent>();
    total += maxHandlesAllocated<ButtonComponent>();
    total += maxHandlesAllocated<InputTouchComponent>();
    total += maxHandlesAllocated<PolyLineRenderComponent>();
    total += maxHandlesAllocated<LineRenderComponent>();
    total += maxHandlesAllocated<FlatRectRenderComponent>();
    total += maxHandlesAllocated<MaskComponent>();
    total += maxHandlesAllocated<TextureMaskComponent>();
    
    return total;
}

uint32_t BGE::ComponentService::numMaterials() const {
    uint32_t num = 0;
    
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == SpriteRenderComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<SpriteRenderComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == TextComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<TextComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == PolyLineRenderComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<PolyLineRenderComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == LineRenderComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<LineRenderComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == FlatRectRenderComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<FlatRectRenderComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == MaskComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<MaskComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        } else if (i == TextureMaskComponent::typeId_) {
            auto const &handles = componentHandles_[i];
            for (auto const &handle : handles) {
                auto component = getComponent<TextureMaskComponent>(handle.handle);
                
                if (component) {
                    num += component->getMaterialHandles().size();
                }
            }
        }
    }

    return num;
}

size_t BGE::ComponentService::usedHandleMemory() const {
    size_t mem = 0;
    
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == TransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<TransformComponent, TransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == BoundingBoxComponent::typeId_) {
            auto handleService = static_cast<HandleService<BoundingBoxComponent, BoundingBoxComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == AnimationChannelComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationChannelComponent, AnimationChannelComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == AnimationSequenceComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationSequenceComponent, AnimationSequenceComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == AnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimatorComponent, AnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == ChannelFrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<ChannelFrameAnimatorComponent, ChannelFrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == ColorMatrixComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorMatrixComponent, ColorMatrixComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == ColorTransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorTransformComponent, ColorTransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == FrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<FrameAnimatorComponent, FrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == SpriteRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<SpriteRenderComponent, SpriteRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == LogicComponent::typeId_) {
            auto handleService = static_cast<HandleService<LogicComponent, LogicComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == PlacementComponent::typeId_) {
            auto handleService = static_cast<HandleService<PlacementComponent, PlacementComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == PolyLineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<PolyLineRenderComponent, PolyLineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == LineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<LineRenderComponent, LineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == FlatRectRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<FlatRectRenderComponent, FlatRectRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == MaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<MaskComponent, MaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == TextureMaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextureMaskComponent, TextureMaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else {
            assert(false);
        }
    }
    
    return mem;
}

size_t BGE::ComponentService::unusedHandleMemory() const {
    size_t mem = 0;
    
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == TransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<TransformComponent, TransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == BoundingBoxComponent::typeId_) {
            auto handleService = static_cast<HandleService<BoundingBoxComponent, BoundingBoxComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == AnimationChannelComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationChannelComponent, AnimationChannelComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == AnimationSequenceComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationSequenceComponent, AnimationSequenceComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == AnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimatorComponent, AnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == ChannelFrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<ChannelFrameAnimatorComponent, ChannelFrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == ColorMatrixComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorMatrixComponent, ColorMatrixComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == ColorTransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorTransformComponent, ColorTransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == FrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<FrameAnimatorComponent, FrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == SpriteRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<SpriteRenderComponent, SpriteRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == LogicComponent::typeId_) {
            auto handleService = static_cast<HandleService<LogicComponent, LogicComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == PlacementComponent::typeId_) {
            auto handleService = static_cast<HandleService<PlacementComponent, PlacementComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == PolyLineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<PolyLineRenderComponent, PolyLineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == LineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<LineRenderComponent, LineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == FlatRectRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<FlatRectRenderComponent, FlatRectRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == MaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<MaskComponent, MaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == TextureMaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextureMaskComponent, TextureMaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else {
            assert(false);
        }
    }
    
    return mem;
}

size_t BGE::ComponentService::totalHandleMemory() const {
    size_t mem = 0;
    
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == TransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<TransformComponent, TransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == BoundingBoxComponent::typeId_) {
            auto handleService = static_cast<HandleService<BoundingBoxComponent, BoundingBoxComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == AnimationChannelComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationChannelComponent, AnimationChannelComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == AnimationSequenceComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimationSequenceComponent, AnimationSequenceComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == AnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<AnimatorComponent, AnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == ChannelFrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<ChannelFrameAnimatorComponent, ChannelFrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == ColorMatrixComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorMatrixComponent, ColorMatrixComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == ColorTransformComponent::typeId_) {
            auto handleService = static_cast<HandleService<ColorTransformComponent, ColorTransformComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == FrameAnimatorComponent::typeId_) {
            auto handleService = static_cast<HandleService<FrameAnimatorComponent, FrameAnimatorComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == SpriteRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<SpriteRenderComponent, SpriteRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == LogicComponent::typeId_) {
            auto handleService = static_cast<HandleService<LogicComponent, LogicComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == PlacementComponent::typeId_) {
            auto handleService = static_cast<HandleService<PlacementComponent, PlacementComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == PolyLineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<PolyLineRenderComponent, PolyLineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == LineRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<LineRenderComponent, LineRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == FlatRectRenderComponent::typeId_) {
            auto handleService = static_cast<HandleService<FlatRectRenderComponent, FlatRectRenderComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == MaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<MaskComponent, MaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == TextureMaskComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextureMaskComponent, TextureMaskComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else {
            assert(false);
        }
    }
    
    return mem;
}

void BGE::ComponentService::outputResourceBreakdown(uint32_t numTabs) const {
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == TransformComponent::typeId_) {
            outputComponentResourceBreakdown<TransformComponent>(numTabs);
        } else if (i == BoundingBoxComponent::typeId_) {
            outputComponentResourceBreakdown<BoundingBoxComponent>(numTabs);
        } else if (i == AnimationChannelComponent::typeId_) {
            outputComponentResourceBreakdown<AnimationChannelComponent>(numTabs);
        } else if (i == AnimationSequenceComponent::typeId_) {
            outputComponentResourceBreakdown<AnimationSequenceComponent>(numTabs);
        } else if (i == AnimatorComponent::typeId_) {
            outputComponentResourceBreakdown<AnimatorComponent>(numTabs);
        } else if (i == ChannelFrameAnimatorComponent::typeId_) {
            outputComponentResourceBreakdown<ChannelFrameAnimatorComponent>(numTabs);
        } else if (i == ColorMatrixComponent::typeId_) {
            outputComponentResourceBreakdown<ColorMatrixComponent>(numTabs);
        } else if (i == ColorTransformComponent::typeId_) {
            outputComponentResourceBreakdown<ColorTransformComponent>(numTabs);
        } else if (i == FrameAnimatorComponent::typeId_) {
            outputComponentResourceBreakdown<FrameAnimatorComponent>(numTabs);
        } else if (i == SpriteRenderComponent::typeId_) {
            outputComponentResourceBreakdown<SpriteRenderComponent>(numTabs);
        } else if (i == LogicComponent::typeId_) {
            outputComponentResourceBreakdown<LogicComponent>(numTabs);
        } else if (i == TextComponent::typeId_) {
            outputComponentResourceBreakdown<TextComponent>(numTabs);
        } else if (i == PlacementComponent::typeId_) {
            outputComponentResourceBreakdown<PlacementComponent>(numTabs);
        } else if (i == ButtonComponent::typeId_) {
            outputComponentResourceBreakdown<ButtonComponent>(numTabs);
        } else if (i == InputTouchComponent::typeId_) {
            outputComponentResourceBreakdown<InputTouchComponent>(numTabs);
        } else if (i == PolyLineRenderComponent::typeId_) {
            outputComponentResourceBreakdown<PolyLineRenderComponent>(numTabs);
        } else if (i == LineRenderComponent::typeId_) {
            outputComponentResourceBreakdown<LineRenderComponent>(numTabs);
        } else if (i == FlatRectRenderComponent::typeId_) {
            outputComponentResourceBreakdown<FlatRectRenderComponent>(numTabs);
        } else if (i == MaskComponent::typeId_) {
            outputComponentResourceBreakdown<MaskComponent>(numTabs);
        } else if (i == TextureMaskComponent::typeId_) {
            outputComponentResourceBreakdown<TextureMaskComponent>(numTabs);
        } else {
            assert(false);
        }
    }
}

void BGE::ComponentService::outputMemoryBreakdown(uint32_t numTabs) const {
    for (auto i=0u;i<componentHandleServices_.size();i++) {
        if (i == TransformComponent::typeId_) {
            outputComponentMemoryBreakdown<TransformComponent>(numTabs);
        } else if (i == BoundingBoxComponent::typeId_) {
            outputComponentMemoryBreakdown<BoundingBoxComponent>(numTabs);
        } else if (i == AnimationChannelComponent::typeId_) {
            outputComponentMemoryBreakdown<AnimationChannelComponent>(numTabs);
        } else if (i == AnimationSequenceComponent::typeId_) {
            outputComponentMemoryBreakdown<AnimationSequenceComponent>(numTabs);
        } else if (i == AnimatorComponent::typeId_) {
            outputComponentMemoryBreakdown<AnimatorComponent>(numTabs);
        } else if (i == ChannelFrameAnimatorComponent::typeId_) {
            outputComponentMemoryBreakdown<ChannelFrameAnimatorComponent>(numTabs);
        } else if (i == ColorMatrixComponent::typeId_) {
            outputComponentMemoryBreakdown<ColorMatrixComponent>(numTabs);
        } else if (i == ColorTransformComponent::typeId_) {
            outputComponentMemoryBreakdown<ColorTransformComponent>(numTabs);
        } else if (i == FrameAnimatorComponent::typeId_) {
            outputComponentMemoryBreakdown<FrameAnimatorComponent>(numTabs);
        } else if (i == SpriteRenderComponent::typeId_) {
            outputComponentMemoryBreakdown<SpriteRenderComponent>(numTabs);
        } else if (i == LogicComponent::typeId_) {
            outputComponentMemoryBreakdown<LogicComponent>(numTabs);
        } else if (i == TextComponent::typeId_) {
            outputComponentMemoryBreakdown<TextComponent>(numTabs);
        } else if (i == PlacementComponent::typeId_) {
            outputComponentMemoryBreakdown<PlacementComponent>(numTabs);
        } else if (i == ButtonComponent::typeId_) {
            outputComponentMemoryBreakdown<ButtonComponent>(numTabs);
        } else if (i == InputTouchComponent::typeId_) {
            outputComponentMemoryBreakdown<InputTouchComponent>(numTabs);
        } else if (i == PolyLineRenderComponent::typeId_) {
            outputComponentMemoryBreakdown<PolyLineRenderComponent>(numTabs);
        } else if (i == LineRenderComponent::typeId_) {
            outputComponentMemoryBreakdown<LineRenderComponent>(numTabs);
        } else if (i == FlatRectRenderComponent::typeId_) {
            outputComponentMemoryBreakdown<FlatRectRenderComponent>(numTabs);
        } else if (i == MaskComponent::typeId_) {
            outputComponentMemoryBreakdown<MaskComponent>(numTabs);
        } else if (i == TextureMaskComponent::typeId_) {
            outputComponentMemoryBreakdown<TextureMaskComponent>(numTabs);
        } else {
            assert(false);
        }
    }
}

void BGE::ComponentService::outputBreakdown(uint32_t numTabs, const char *format, ...) const {
    va_list args;
    va_start(args, format);
    
    Game::outputValue(numTabs, format, args);
    
    va_end(args);
}

