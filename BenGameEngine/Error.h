//
//  Error.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEError_h
#define BGEError_h

#include <stdio.h>
#include <memory>
#include <string>

namespace BGE {    
    class Error
    {
    public:
        Error(std::string domain, int32_t errorCode);
        Error(std::string domain, int32_t errorCode, std::shared_ptr<Error> underlyingError);
        
        std::string getDomain() const { return domain_; }
        int32_t getErrorCode() const { return errorCode_; }
        std::shared_ptr<Error> getUnderlyingError() { return underlyingError_; }
        
    private:
        std::string domain_;
        int32_t    errorCode_;
        std::shared_ptr<Error> underlyingError_;
    };
}

#endif /* BGEError_h */
