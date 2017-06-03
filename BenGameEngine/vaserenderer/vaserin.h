//
//  vaserin.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/1/17.
//  Copyright © 2017 2n Productions. All rights reserved.
//

#ifndef vaserin_h
#define vaserin_h

#include "vaser.h"

namespace VASEr {
    namespace VASErin {
        struct polyline_inopt
        {
            bool const_color;
            bool const_weight;
            bool no_cap_first;
            bool no_cap_last;
            bool join_first;
            bool join_last;
            double* segment_length; //array of length of each segment
        };

        extern Color Color_between( const Color& A, const Color& B, float t=0.5f);
        extern bool Color_valid_range(float t);
        extern void gradient_apply(const gradient* gradp, Color* C, double* W, const double* L, int length, double path_length);
        extern void curve4_div(double x1, double y1,
                               double x2, double y2,
                               double x3, double y3,
                               double x4, double y4,
                               double m_approximation_scale,
                               double m_angle_tolerance,
                               double m_cusp_limit,
                               void (*add_point)(void*,double,double),
                               void* obj );
        void polyline(
                      const Vec2* PP,  //pointer to array of point of a polyline
                      const Color* C,  //array of color
                      const double* W, //array of weight
                      int length, //size of the buffer P
                      const polyline_opt* options, //options
                      const polyline_inopt* in_options);
    }
}

#endif /* vaserin_h */
