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
#include "SpriteRenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "ButtonComponent.h"
#include "MaskComponent.h"
#include "TextureMaskComponent.h"
#include "InputTouchComponent.h"
#include "BoundingBoxComponent.h"

bool BGE::ComponentService::componentsRegistered_ = false;
std::vector<void *> BGE::ComponentService::componentHandleServices_;

void BGE::ComponentService::registerComponents() {
    if (!ComponentService::componentsRegistered_) {
        registerComponent<TransformComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<BoundingBoxComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimationChannelComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimationSequenceComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<AnimatorComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<ChannelFrameAnimatorComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<ColorMatrixComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<ColorTransformComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<FrameAnimatorComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<SpriteRenderComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<TextComponent>(120, HandleServiceNoMaxLimit);
        registerComponent<ButtonComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<InputTouchComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<LineRenderComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<FlatRectRenderComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<MaskComponent>(1024, HandleServiceNoMaxLimit);
        registerComponent<TextureMaskComponent>(1024, HandleServiceNoMaxLimit);
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

BGE::ComponentService::ComponentService(SpaceHandle spaceHandle) : Service(), spaceHandle_(spaceHandle) {
    if (!ComponentService::componentsRegistered_) {
        ComponentService::registerComponents();
    }
    
    // Create our componentHandle vectors
    for (auto it=componentHandleServices_.begin();it!=componentHandleServices_.end();++it) {
        componentHandles_.push_back(std::vector<ComponentHandle>());
    }
}

BGE::GameObject *BGE::ComponentService::getComponentGameObject(Component *component, GameObjectHandle gameObjHandle) {
    return component->getSpace()->getGameObject(gameObjHandle);
}

void BGE::ComponentService::removeComponent(ComponentHandle handle) {
    auto typeId = handle.typeId;
    auto &handles = componentHandles_[typeId];
    
    for (auto h=handles.begin();h!=handles.end();++h) {
        if (h->handle == handle.handle) {
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
            } else if (typeId == TextComponent::typeId_) {
                releaseComponentHandle<TextComponent>(handle);
            } else if (typeId == ButtonComponent::typeId_) {
                releaseComponentHandle<ButtonComponent>(handle);
            } else if (typeId == InputTouchComponent::typeId_) {
                releaseComponentHandle<InputTouchComponent>(handle);
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

            handles.erase(h);
            return;
        }
    }
}

void BGE::ComponentService::removeAllComponents() {
    for (auto &handles : componentHandles_) {
        for (auto const &handle : handles) {
            removeComponent(handle);
        }
        
        handles.clear();
    }
}

uint32_t BGE::ComponentService::totalNumComponent() const {
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
    total += numComponents<TextComponent>();
    total += numComponents<ButtonComponent>();
    total += numComponents<InputTouchComponent>();
    total += numComponents<LineRenderComponent>();
    total += numComponents<FlatRectRenderComponent>();
    total += numComponents<MaskComponent>();
    total += numComponents<TextureMaskComponent>();
    
    return total;
}

size_t BGE::ComponentService::usedHandleMemory() const {
    size_t mem = 0;
    
    for (auto i=0;i<componentHandleServices_.size();i++) {
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
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->usedMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
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
    
    for (auto i=0;i<componentHandleServices_.size();i++) {
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
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->unusedMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
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
    
    for (auto i=0;i<componentHandleServices_.size();i++) {
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
        } else if (i == TextComponent::typeId_) {
            auto handleService = static_cast<HandleService<TextComponent, TextComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == ButtonComponent::typeId_) {
            auto handleService = static_cast<HandleService<ButtonComponent, ButtonComponentHandle> *>(componentHandleServices_[i]);
            mem += handleService->totalMemory();
        } else if (i == InputTouchComponent::typeId_) {
            auto handleService = static_cast<HandleService<InputTouchComponent, InputTouchComponentHandle> *>(componentHandleServices_[i]);
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

