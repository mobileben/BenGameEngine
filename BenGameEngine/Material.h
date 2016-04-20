//
//  Material.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEMaterial_h
#define BGEMaterial_h

#include <stdio.h>
#include <memory>
#include <string>
#include "Object.h"
#include "BGEMathTypes.h"
#include "BGETextureBase.h"

namespace BGE {
    class MaterialService;
    
    class Material : public BGE::Object {
    public:
        void getColor(BGEColor& color) const { color = color_; }
        void setColor(BGEColor& color);
        void getColorMatrix(BGEColor& colorMatrix) const;
        std::weak_ptr<BGETextureBase> getTexture() const { return texture_; }
        void setTexture(std::shared_ptr<BGETextureBase> texture) { texture_ = texture; }
        
    protected:
        friend MaterialService;
        
        Material(uint64_t matId);
        Material(uint64_t matId, std::string name);
        Material(uint64_t matId, std::string name, BGEColor& color);
        Material(uint64_t matId, std::string name, std::shared_ptr<BGETextureBase> texture);
        Material(uint64_t matId, std::string name, BGEColor& color, std::shared_ptr<BGETextureBase> texture);
        
    private:
        bool                        colorDirty_;
        BGEVector4                  color_;
        BGEMatrix4                  colorMatrix_;
        
        std::weak_ptr<BGETextureBase>   texture_;
    };
}

#endif /* BGEMaterial_h */
