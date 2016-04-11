//
//  BGEMaterial.h
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
#include "BGEObject.h"
#include "BGEMathTypes.h"
#include "BGETexture.h"

class BGEMaterial : public BGEObject {
public:
    BGEMaterial(uint64_t obj);
    BGEMaterial(uint64_t obj, std::string name);
    BGEMaterial(uint64_t obj, std::string name, BGEVector4& color);
    BGEMaterial(uint64_t obj, std::string name, std::shared_ptr<BGETexture> texture);
    BGEMaterial(uint64_t obj, std::string name, BGEVector4& color, std::shared_ptr<BGETexture> texture);
    
    void getColor(BGEVector4& color) const;
    void setColor(BGEVector4& color);
    void getColorMatrix(BGEMatrix4& colorMatrix) const;
    std::weak_ptr<BGETexture> getTexture() const { return texture_; }
    void setTexture(std::shared_ptr<BGETexture> texture) { texture_ = texture; }
    
private:
    bool                        colorDirty_;
    BGEVector4                  color_;
    BGEMatrix4                  colorMatrix_;
    
    std::weak_ptr<BGETexture>   texture_;
};

#endif /* BGEMaterial_h */
