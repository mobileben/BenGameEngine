//
//  StringArrayBuilder.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/1/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef StringArray_h
#define StringArray_h

#include <stdio.h>
#include <string>
#ifdef UNIQUE_STRING_ARRAY
#include "UniqueArrayBuilder.h"
#else
#include "ArrayBuilder.h"
#endif

namespace BGE {
#ifdef UNIQUE_STRING_ARRAY
    class StringArrayBuilder : public UniqueArrayBuilder<std::string, char> {
#else
    class StringArrayBuilder : public ArrayBuilder<std::string, char> {
#endif
    public:
        size_t add(const std::string& item) {
            return add(item.c_str());
        }

        size_t add(const char *item) {
            size_t start;
            
#ifdef UNIQUE_STRING_ARRAY
            if (indices_.find(item) != indices_.end()) {
                start = indices_[item];
            } else {
                start = items_.size();
                items_.insert(items_.end(), item, item + strlen(item) + 1);
                indices_[item] = start;
            }
#else
            start = items_.size();
            items_.insert(items_.end(), item, item + strlen(item) + 1);
#endif
            
            return start;
        }
        
        const char* testRef() {
            return (const char *) &items_[0];
        }
    };
}

#endif /* StringArray_h */
