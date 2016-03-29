//
//  BGEError.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/20/16.
//  Copyright © 2016 Benjamin Lee. All rights reserved.
//

#include "BGEError.h"

BGEError::BGEError(std::string domain, int32_t errorCode) : domain_(domain), errorCode_(errorCode) {
}

BGEError::BGEError(std::string domain, int32_t errorCode, std::shared_ptr<BGEError> underlyingError) : domain_(domain), errorCode_(errorCode), underlyingError_(underlyingError) {
}
