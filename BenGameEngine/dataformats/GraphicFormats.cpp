//
//  GraphicFormats.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/3/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "GraphicFormats.h"

bool BGE::operator==(const TextReference& lhs, const TextReference& rhs) {
    if (lhs.name != rhs.name) {
        if (lhs.name) {
            if (!rhs.name) {
                return false;
            }
            
            if (strcmp(lhs.name, rhs.name)) {
                return false;
            }
        }
    }

    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.color != rhs.color) {
        return false;
    }
    
    if (lhs.alignment != rhs.alignment) {
        return false;
    }
    
    if (lhs.font != rhs.font) {
        return false;
    }
    
    return true;
}

bool BGE::operator!=(const TextReference& lhs, const TextReference& rhs) {
    if (lhs.name != rhs.name) {
        if (lhs.name) {
            if (!rhs.name) {
                return true;
            }
            
            if (strcmp(lhs.name, rhs.name)) {
                return true;
            }
        }
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.color != rhs.color) {
        return true;
    }
    
    if (lhs.alignment != rhs.alignment) {
        return true;
    }
    
    if (lhs.font != rhs.font) {
        return true;
    }
    
    return false;
}

bool BGE::operator==(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs) {
    if (lhs.name != rhs.name) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.color != rhs.color) {
        return false;
    }
    
    if (lhs.alignment != rhs.alignment) {
        return false;
    }
    
    return true;
}

bool BGE::operator!=(const TextReferenceIntermediate& lhs, const TextReferenceIntermediate& rhs) {
    if (lhs.name != rhs.name) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.color != rhs.color) {
        return true;
    }
    
    if (lhs.alignment != rhs.alignment) {
        return true;
    }
    
    return false;
}

bool BGE::operator==(const TextureReference& lhs, const TextureReference& rhs) {
    if (lhs.name != rhs.name) {
        if (lhs.name) {
            if (!rhs.name) {
                return false;
            }
            
            if (strcmp(lhs.name, rhs.name)) {
                return false;
            }
        }
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.texture != rhs.texture) {
        return false;
    }
    
    return true;
}

bool BGE::operator!=(const TextureReference& lhs, const TextureReference& rhs) {
    if (lhs.name != rhs.name) {
        if (lhs.name) {
            if (!rhs.name) {
                return true;
            }
            
            if (strcmp(lhs.name, rhs.name)) {
                return true;
            }
        }
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.texture != rhs.texture) {
        return true;
    }
    
    return false;
}

bool BGE::operator==(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs) {
    if (lhs.name != rhs.name) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    if (lhs.height != rhs.height) {
        return false;
    }
    
    return true;
}

bool BGE::operator!=(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs) {
    if (lhs.name != rhs.name) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    if (lhs.height != rhs.height) {
        return true;
    }
    
    return false;
}

