//
//  Handle.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef Handle_h
#define Handle_h

#include <stdio.h>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace BGE {
    using HandleBackingType = uint32_t;
    
    class GameObject;
    class EventHandler;
    
    class AnimationChannelComponent;
    class AnimationSequenceComponent;
    class AnimatorComponent;
    class BoundingBoxComponent;
    class ButtonComponent;
    class ChannelFrameAnimatorComponent;
    class ColorMatrixComponent;
    class ColorTransformComponent;
    class FlatRectRenderComponent;
    class FrameAnimatorComponent;
    class InputTouchComponent;
    class LineRenderComponent;
    class LogicComponent;
    class MaskComponent;
    class PlacementComponent;
    class SpriteRenderComponent;
    class TextComponent;
    class TextureMaskComponent;
    class TransformComponent;
    class Audio;
    class AudioBuffer;
    
    template <typename TAG>
    class Handle {
    public:
        static const HandleBackingType NUM_INDEX_BITS = 18;
        static const HandleBackingType NUM_MAGIC_BITS = 14;
        static const HandleBackingType MAX_INDEX = (1 << NUM_INDEX_BITS) - 1;
        static const HandleBackingType MAX_MAGIC = (1 << NUM_MAGIC_BITS) - 1;

        Handle() : handle_(0) {}
        Handle(HandleBackingType h) : handle_(h) {
        }

        HandleBackingType getIndex() const { return index_; }
        HandleBackingType getMagic() const { return magic_; }
        HandleBackingType getHandle() const { return handle_; }
        
        inline bool isNull() const { return !handle_; }
        
        inline void init(HandleBackingType index) {
            assert(isNull());
            assert(index < MAX_INDEX);
            
            static HandleBackingType autoMagic = 0;
            
            if (++autoMagic > MAX_MAGIC) {
                autoMagic = 1;
            }
            
            index_ = index;
            magic_ = autoMagic;
        }
        
        inline void nullify() {
            handle_ = 0;
        }
        
    private:
        union {
            struct {
                HandleBackingType index_ : NUM_INDEX_BITS;
                HandleBackingType magic_: NUM_MAGIC_BITS;
            };
            HandleBackingType handle_;
        };
    };

    template <typename TAG>
    inline bool operator!= (const Handle<TAG>& lhs, const Handle<TAG>& rhs) { return lhs.getHandle() != rhs.getHandle(); }
    template <typename TAG>
    inline bool operator== (const Handle<TAG>& lhs, const Handle<TAG>& rhs) { return lhs.getHandle() == rhs.getHandle(); }
    
    // Define handles here, so they can be used anywhere
    using EventHandlerHandle = Handle<EventHandler>;

    struct FontTag {};
    using FontHandle = Handle<FontTag>;
    
    using GameObjectHandle = Handle<GameObject>;
    
    struct InputTag {};
    using InputHandle = Handle<InputTag>;
    
    struct MaterialTag {};
    using MaterialHandle = Handle<MaterialTag>;
    
    struct ScenePackageTag {};
    using ScenePackageHandle = Handle<ScenePackageTag>;

    struct SpaceTag {};
    using SpaceHandle = Handle<SpaceTag>;
    
    struct TextureTag {};
    using TextureHandle = Handle<TextureTag>;
    
    struct TextureAtlasTag {};
    using TextureAtlasHandle = Handle<TextureAtlasTag>;
    
    using AnimationChannelComponentHandle = Handle<AnimationChannelComponent>;
    using AnimationSequenceComponentHandle = Handle<AnimationSequenceComponent>;
    using AnimatorComponentHandle = Handle<AnimatorComponent>;
    using AudioHandle = Handle<Audio>;
    using AudioBufferHandle = Handle<AudioBuffer>;
    using BoundingBoxComponentHandle = Handle<BoundingBoxComponent>;
    using ButtonComponentHandle = Handle<ButtonComponent>;
    using ChannelFrameAnimatorComponentHandle = Handle<ChannelFrameAnimatorComponent>;
    using ColorMatrixComponentHandle = Handle<ColorMatrixComponent>;
    using ColorTransformComponentHandle = Handle<ColorTransformComponent>;
    using FlatRectRenderComponentHandle = Handle<FlatRectRenderComponent>;
    using FrameAnimatorComponentHandle = Handle<FrameAnimatorComponent>;
    using InputTouchComponentHandle = Handle<InputTouchComponent>;
    using LineRenderComponentHandle = Handle<LineRenderComponent>;
    using LogicComponentHandle = Handle<LogicComponent>;
    using MaskComponentHandle = Handle<MaskComponent>;
    using PlacementComponentHandle = Handle<PlacementComponent>;
    using SpriteRenderComponentHandle = Handle<SpriteRenderComponent>;
    using TextComponentHandle = Handle<TextComponent>;
    using TextureMaskComponentHandle = Handle<TextureMaskComponent>;
    using TransformComponentHandle = Handle<TransformComponent>;
    
    struct ComponentHandle {
        uint32_t            typeId;
        HandleBackingType   handle;
    };
}

template<typename TAG>
struct std::hash<BGE::Handle<TAG>> {
    size_t operator()(const BGE::Handle<TAG> handle) const {
        return handle.getHandle();
    }
};

#endif /* Handle_h */
