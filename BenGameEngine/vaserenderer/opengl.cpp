/* OpenGL 1.1 renderer and backend */
#include "vaser.h"
#include "backend.h"
#include "vertex_array_holder.h"
#include <OpenGLES/ES2/glext.h>

namespace VASEr {
    namespace VASErin {	//VASEr internal namespace
        GLint backend::positionLocation_ = -1;
        GLint backend::colorLocation_ = -1;

        void backend::set_vertex_attributes(GLint positionLocation, GLint colorLocation) {
            backend::positionLocation_ = positionLocation;
            backend::colorLocation_ = colorLocation;
        }

        void backend::vah_draw(vertex_array_holder& vah)
        {
            if ( vah.count > 0) //save some effort
            {
#ifdef SUPPORT_OPENGL_1_1
                glVertexPointer(2, GL_FLOAT, 0, &vah.vert[0]);
                glColorPointer (4, GL_FLOAT, 0, &vah.color[0]);
                glDrawArrays (vah.glmode, 0, vah.count);
#else
                glVertexAttribPointer(backend::colorLocation_, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLfloat *) &vah.color[0]);
                // 2
                glVertexAttribPointer(backend::positionLocation_, 2, GL_FLOAT, GL_FALSE,
                                      sizeof(float) * 2, &vah.vert[0]);
                glDrawArrays (vah.glmode, 0, vah.count);
#endif /* SUPPORT_OPENGL_1_1 */
            }
        }
        
        void backend::polyline(__attribute__ ((unused)) const Vec2* P, __attribute__ ((unused)) Color C, __attribute__ ((unused)) double W, __attribute__ ((unused)) int length, const polyline_opt*) //constant color and weight
        {
#ifdef NOT_YET
            int type=0;
            if( sizeof(Vec2)==16)
                type = GL_DOUBLE;
            else if( sizeof (Vec2)==8)
                type = GL_FLOAT;
            
            glColor4f(C.r,C.g,C.b,C.a);
            glLineWidth(W);
            if( type)
            {
                glEnableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glVertexPointer(2, type, 0, P);
                glDrawArrays(GL_LINE_STRIP, 0, length);
                glEnableClientState(GL_COLOR_ARRAY);
            }
            else
            {
                glBegin(GL_LINE_STRIP);
                for( int i=0; i<length; i++)
                    glVertex2f(P[i].x,P[i].y);
                glEnd();
            }
            glLineWidth(1);
#endif
        }
        
        void swap_pixel(unsigned char* img, int w, int x1,int y1, int x2,int y2)
        {
            const int b=4;
            unsigned char R = img[y1*w*b+x1*b];
            unsigned char G = img[y1*w*b+x1*b+1];
            unsigned char B = img[y1*w*b+x1*b+2];
            unsigned char A = img[y1*w*b+x1*b+3];
            
            img[y1*w*b+x1*b] = img[y2*w*b+x2*b];
            img[y1*w*b+x1*b+1] = img[y2*w*b+x2*b+1];
            img[y1*w*b+x1*b+2] = img[y2*w*b+x2*b+2];
            img[y1*w*b+x1*b+3] = img[y2*w*b+x2*b+3];
            
            img[y2*w*b+x2*b] = R;
            img[y2*w*b+x2*b+1] = G;
            img[y2*w*b+x2*b+2] = B;
            img[y2*w*b+x2*b+3] = A;
        }
        
    } //sub namespace VASErin
    
    void renderer::init()
    {
    }
    
    void renderer::before()
    {
#ifdef NOT_YET
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        glDisableClientState(GL_EDGE_FLAG_ARRAY);
        //glDisableClientState(GL_FOG_COORD_ARRAY);
        glDisableClientState(GL_INDEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        //glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif /* NOT_YET */
    }
    void renderer::after()
    {
#ifdef NOT_YET
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisable(GL_BLEND);
#endif /* NOT_YET */
    }
    Image renderer::get_image()
    {
        Image im={nullptr, 0, 0};
#ifdef NOT_YET
        int res[4];
        glGetIntegerv(GL_VIEWPORT,res);
        im.width=res[2];
        im.height=res[3];
        im.buf=(unsigned char*)malloc(im.width*im.height*4);
        if( !im.buf)
            return im;
        glReadPixels(0, 0, im.width, im.height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)im.buf);
        //flip horizontally
        for( int i=0; i<im.height/2; i++)
            for( int j=0; j<im.width; j++)
                VASErin::swap_pixel(im.buf,im.width,j,i,j,im.height-i-1);
#endif /* NOT_YET */
        return im;
    }
}

