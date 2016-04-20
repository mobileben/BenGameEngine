//
//  Error.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/20/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "Error.h"

BGE::Error::Error(std::string domain, int32_t errorCode) : domain_(domain), errorCode_(errorCode) {
}

BGE::Error::Error(std::string domain, int32_t errorCode, std::shared_ptr<Error> underlyingError) : domain_(domain), errorCode_(errorCode), underlyingError_(underlyingError) {
}
