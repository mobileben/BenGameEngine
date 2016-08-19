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

namespace BGE {
    using HandleBackingType = uint32_t;
    class TransformComponent;
    
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
        
        bool isNull() const { return !handle_; }
        
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
    struct EventHandlerTag {};
    using EventHandlerHandle = Handle<EventHandlerTag>;

    struct FontTag {};
    using FontHandle = Handle<FontTag>;
    
    struct GameObjectTag {};
    using GameObjectHandle = Handle<GameObjectTag>;
    
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
    
    struct AnimationChannelComponentTag {};
    using AnimationChannelComponentHandle = Handle<AnimationChannelComponentTag>;
    
    struct AnimationSequenceComponentTag {};
    using AnimationSequenceComponentHandle = Handle<AnimationSequenceComponentTag>;
    
    struct AnimatorComponentTag {};
    using AnimatorComponentHandle = Handle<AnimatorComponentTag>;
    
    struct BoundingBoxComponentTag {};
    using BoundingBoxComponentHandle = Handle<BoundingBoxComponentTag>;
    
    struct ButtonComponentTag {};
    using ButtonComponentHandle = Handle<ButtonComponentTag>;
    
    struct ChannelFrameAnimatorComponentTag {};
    using ChannelFrameAnimatorComponentHandle = Handle<ChannelFrameAnimatorComponentTag>;
    
    struct ColorMatrixComponentTag {};
    using ColorMatrixComponentHandle = Handle<ColorMatrixComponentTag>;
    
    struct ColorTransformComponentTag {};
    using ColorTransformComponentHandle = Handle<ColorTransformComponentTag>;
    
    struct FlatRectRenderComponentTag {};
    using FlatRectRenderComponentHandle = Handle<FlatRectRenderComponentTag>;
    
    struct FrameAnimatorComponentTag {};
    using FrameAnimatorComponentHandle = Handle<FrameAnimatorComponentTag>;
    
    struct InputTouchComponentTag {};
    using InputTouchComponentHandle = Handle<InputTouchComponentTag>;
    
    struct LineRenderComponentTag {};
    using LineRenderComponentHandle = Handle<LineRenderComponentTag>;
    
    struct MaskComponentTag {};
    using MaskComponentHandle = Handle<MaskComponentTag>;
    
    struct SpriteRenderComponentTag {};
    using SpriteRenderComponentHandle = Handle<SpriteRenderComponentTag>;
    
    struct TextComponentTag {};
    using TextComponentHandle = Handle<TextComponentTag>;
    
    struct TextureMaskComponentTag {};
    using TextureMaskComponentHandle = Handle<TextureMaskComponentTag>;
    
    struct TransformComponentTag {};
    using TransformComponentHandle = Handle<TransformComponentTag>;
    
    using TransformComponent2Handle = Handle<TransformComponent>;
    
    struct ComponentHandle {
        uint32_t            bitmask;
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
