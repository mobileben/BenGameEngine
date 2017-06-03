#ifndef __VASER_BACKEND_H__
#define __VASER_BACKEND_H__

#include "vaser.h"
#include <OpenGLES/ES2/glext.h>

namespace VASEr {
    namespace VASErin {
        class vertex_array_holder;
        
        class backend
        {
        public:
            static void set_uniforms(GLint positionLocation, GLint colorLocation);
            static void vah_draw(vertex_array_holder& vah);
            static void polyline( const Vec2*, Color, double W, int length, const polyline_opt*); //constant color and weight
            
        private:
            static GLint positionLocation_;
            static GLint colorLocation_;
        };
    }
}

#endif /* __VASER_BACKEND_H__ */
