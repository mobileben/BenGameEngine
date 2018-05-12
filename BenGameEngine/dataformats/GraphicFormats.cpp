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
    
    if (lhs.multiline != rhs.multiline) {
        return false;
    }
    
    if (lhs.fontHandle != rhs.fontHandle) {
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
    
    if (lhs.multiline != rhs.multiline) {
        return true;
    }

    if (lhs.fontHandle != rhs.fontHandle) {
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
    
    if (lhs.multiline != rhs.multiline) {
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
    
    if (lhs.multiline != rhs.multiline) {
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

    if (lhs.textureHandle != rhs.textureHandle) {
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

    if (lhs.textureHandle != rhs.textureHandle) {
        return true;
    }
    
    return false;
}

bool BGE::operator==(const TextureReferenceIntermediate& lhs, const TextureReferenceIntermediate& rhs) {
    if (lhs.name != rhs.name) {
        return false;
    }

    if (lhs.filename != rhs.filename) {
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

    if (lhs.filename != rhs.filename) {
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

bool BGE::operator==(const AnimationKeyframeReference& lhs, const AnimationKeyframeReference& rhs) {
    // For now ignore startFrames and totalFrames
#ifdef NOT_YET
    if (lhs.startFrame != rhs.startFrame) {
        return false;
    }
    if (lhs.totalFrames !+ rhs.totalFrames) {
        return false;
    }
#endif
    if (lhs.order != rhs.order) {
        return false;
    }
    if (lhs.flags != rhs.flags) {
        return false;
    }
    if (lhs.frame != rhs.frame) {
        return false;
    }
    if (lhs.position) {
        if (rhs.position) {
            if (*lhs.position != *rhs.position) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.position) {
        return false;
    }
    if (lhs.scale) {
        if (rhs.scale) {
            if (*lhs.scale != *rhs.scale) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.scale) {
        return false;
    }
    if (lhs.skew) {
        if (rhs.skew) {
            if (*lhs.skew != *rhs.skew) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.skew) {
        return false;
    }
    if (lhs.collisionRectScale) {
        if (rhs.collisionRectScale) {
            if (*lhs.collisionRectScale != *rhs.collisionRectScale) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.collisionRectScale) {
        return false;
    }
    if (lhs.rotation != rhs.rotation) {
        return false;
    }
    if (lhs.colorMatrix) {
        if (rhs.colorMatrix) {
            if (*lhs.colorMatrix != *rhs.colorMatrix) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.colorMatrix) {
        return false;
    }
    if (lhs.colorTransform) {
        if (rhs.colorTransform) {
            if (*lhs.colorTransform != *rhs.colorTransform) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.collisionRectScale) {
        return false;
    }
    if (lhs.bounds) {
        if (rhs.bounds) {
            if (*lhs.bounds != *rhs.bounds) {
                return false;
            }
        } else {
            return false;
        }
    } else if (rhs.bounds) {
        return false;
    }
    return true;
}

bool BGE::operator!=(const AnimationKeyframeReference& lhs, const AnimationKeyframeReference& rhs) {
    // For now ignore startFrames and totalFrames
#ifdef NOT_YET
    if (lhs.startFrame != rhs.startFrame) {
        return true;
    }
    if (lhs.totalFrames !+ rhs.totalFrames) {
        return true;
    }
#endif
    if (lhs.order != rhs.order) {
        return true;
    }
    if (lhs.flags != rhs.flags) {
        return true;
    }
    if (lhs.frame != rhs.frame) {
        return true;
    }
    if (lhs.position) {
        if (rhs.position) {
            if (*lhs.position != *rhs.position) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.position) {
        return true;
    }
    if (lhs.scale) {
        if (rhs.scale) {
            if (*lhs.scale != *rhs.scale) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.scale) {
        return true;
    }
    if (lhs.skew) {
        if (rhs.skew) {
            if (*lhs.skew != *rhs.skew) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.skew) {
        return true;
    }
    if (lhs.collisionRectScale) {
        if (rhs.collisionRectScale) {
            if (*lhs.collisionRectScale != *rhs.collisionRectScale) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.collisionRectScale) {
        return true;
    }
    if (lhs.rotation != rhs.rotation) {
        return true;
    }
    if (lhs.colorMatrix) {
        if (rhs.colorMatrix) {
            if (*lhs.colorMatrix != *rhs.colorMatrix) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.colorMatrix) {
        return true;
    }
    if (lhs.colorTransform) {
        if (rhs.colorTransform) {
            if (*lhs.colorTransform != *rhs.colorTransform) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.collisionRectScale) {
        return true;
    }
    if (lhs.bounds) {
        if (rhs.bounds) {
            if (*lhs.bounds != *rhs.bounds) {
                return true;
            }
        } else {
            return true;
        }
    } else if (rhs.bounds) {
        return true;
    }
    return false;
}

bool BGE::operator==(const AnimationKeyframeReferenceIntermediate& lhs, const AnimationKeyframeReferenceIntermediate& rhs) {
    // For now ignore startFrames and totalFrames
#ifdef NOT_YET
    if (lhs.startFrame != rhs.startFrame) {
        return false;
    }
    if (lhs.totalFrames !+ rhs.totalFrames) {
        return false;
    }
#endif
    if (lhs.order != rhs.order) {
        return false;
    }
    if (lhs.flags != rhs.flags) {
        return false;
    }
    if (lhs.frame != rhs.frame) {
        return false;
    }
    if (lhs.position != rhs.position) {
        return false;
    }
    if (lhs.scale != rhs.scale) {
        return false;
    }
    if (lhs.skew != rhs.skew) {
        return false;
    }
    if (lhs.collisionRectScale != rhs.collisionRectScale) {
        return false;
    }
    if (lhs.rotation != rhs.rotation) {
        return false;
    }
#ifdef NOT_YET
    if (lhs.matrix != rhs.matrix) {
        return false;
    }
#endif
    if (lhs.colorMatrix != rhs.colorMatrix) {
        return false;
    }
    if (lhs.colorTransform != rhs.colorTransform) {
        return false;
    }
    if (lhs.bounds != rhs.bounds) {
        return false;
    }
    return true;}

bool BGE::operator!=(const AnimationKeyframeReferenceIntermediate& lhs, const AnimationKeyframeReferenceIntermediate& rhs) {
    // For now ignore startFrames and totalFrames
#ifdef NOT_YET
    if (lhs.startFrame != rhs.startFrame) {
        return true;
    }
    if (lhs.totalFrames !+ rhs.totalFrames) {
        return true;
    }
#endif
    if (lhs.order != rhs.order) {
        return true;
    }
    if (lhs.flags != rhs.flags) {
        return true;
    }
    if (lhs.frame != rhs.frame) {
        return true;
    }
    if (lhs.position != rhs.position) {
        return true;
    }
    if (lhs.scale != rhs.scale) {
        return true;
    }
    if (lhs.skew != rhs.skew) {
        return true;
    }
    if (lhs.collisionRectScale != rhs.collisionRectScale) {
        return true;
    }
    if (lhs.rotation != rhs.rotation) {
        return true;
    }
#ifdef NOT_YET
    if (lhs.matrix != rhs.matrix) {
        return true;
    }
#endif
    if (lhs.colorMatrix != rhs.colorMatrix) {
        return true;
    }
    if (lhs.colorTransform != rhs.colorTransform) {
        return true;
    }
    if (lhs.bounds != rhs.bounds) {
        return true;
    }
    return false;
}


