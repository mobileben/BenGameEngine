#ifndef VASER_CPP
#define VASER_CPP

#include "vaser.h"

#include <math.h>
#include <vector>
#include <stdlib.h>

namespace VASEr
{
    namespace VASErin
    {	//VASEr internal namespace
        const double vaser_min_alw=0.00000000001; //smallest value not regarded as zero
        const Color default_color = {0,0,0,1};
        const double default_weight = 1.0;
    }
} //namespace VASEr

#endif
