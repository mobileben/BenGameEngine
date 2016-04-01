//
//  BGEService.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/9/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEService_h
#define BGEService_h

#include <stdio.h>

class BGEService
{
public:
    BGEService() {}
    virtual ~BGEService() {}
    
    virtual void initialize() =0;
    virtual void reset() =0;
    virtual void enteringBackground() =0;
    virtual void enteringForeground() =0;
    virtual void pause() =0;
    virtual void resume() =0;
    virtual void destroy() =0;
};

#endif /* BGEService_h */
