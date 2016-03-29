//
//  BGEError.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/20/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#ifndef BGEError_h
#define BGEError_h

#include <stdio.h>
#include <memory>
#include <string>

class BGEError
{
public:
    BGEError(std::string domain, int32_t errorCode);
    BGEError(std::string domain, int32_t errorCode, std::shared_ptr<BGEError> underlyingError);
    
    std::string getDomain() const { return domain_; }
    int32_t getErrorCode() const { return errorCode_; }
    std::shared_ptr<BGEError> getUnderlyingError() { return underlyingError_; }
    
private:
    std::string domain_;
    int32_t    errorCode_;
    std::shared_ptr<BGEError> underlyingError_;
};

#endif /* BGEError_h */
