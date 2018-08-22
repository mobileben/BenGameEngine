//
//  FileUtilities.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "FileUtilities.h"
#include <cassert>

bool BGE::FileUtilities::initialized_;
std::string BGE::FileUtilities::runtimePath_;
std::string BGE::FileUtilities::builtinPath_;
std::string BGE::FileUtilities::separator_;

std::string BGE::FileUtilities::getRuntimePath() {
    assert(initialized_);
    
    return runtimePath_;
}

std::string BGE::FileUtilities::getBuiltinPath() {
    assert(initialized_);
    
    return builtinPath_;
}

std::string BGE::FileUtilities::getPath(const BaseDirectory &base) {
    assert(initialized_);

    return base.filename();
}

std::string BGE::FileUtilities::getPath(const FilePath &filePath) {
    assert(initialized_);
    
    return filePath.filename();
}

void BGE::FileUtilities::initialize(std::string builtinPath, std::string runtimePath, std::string separator) {
    runtimePath_ = runtimePath;
    builtinPath_ = builtinPath;
    separator_ = separator;
    
    initialized_ = true;
}

BGE::BaseDirectory::BaseDirectory(std::string path) {
    auto refPath = FileUtilities::getBuiltinPath();
    auto index = path.find(refPath);
    
    if (index != std::string::npos) {
        type = FileUtilities::PathType::builtin;
        
        auto len = index + refPath.length();
        
        if (path.substr(len, 1) == FileUtilities::getSeparator()) {
            len++;
        }
        
        subpath = path.substr(len, path.length() - len);
        
        index = subpath.find_last_of(FileUtilities::getSeparator());
        
        subpath = subpath.substr(0, index);
    } else {
        // This must be runtimePath
        refPath = FileUtilities::getRuntimePath();
        index = path.find(refPath);
        
        if (index != std::string::npos) {
            type = FileUtilities::PathType::runtime;
            
            auto len = index + refPath.length();
            
            if (path.substr(len, 1) == FileUtilities::getSeparator()) {
                len++;
            }
            
            subpath = path.substr(len, path.length() - len);
            
            index = subpath.find_last_of(FileUtilities::getSeparator());
            
            subpath = subpath.substr(0, index);
        } else {
            type = FileUtilities::PathType::builtin;
        }
    }
}

std::string BGE::BaseDirectory::filename() const {
    auto separator = FileUtilities::getSeparator();
    
    std::string path;
    
    if (type == FileUtilities::PathType::builtin) {
        path += FileUtilities::getBuiltinPath();
    } else {
        path += FileUtilities::getRuntimePath();
    }
    
    if (!subpath.empty()) {
        assert(subpath.compare(0, 1, separator) != 0);
        assert(subpath.compare(subpath.length() - 1, 1, separator) != 0);
        path += separator + subpath;
    }
    
    return path;
}

std::string BGE::FilePath::filename() const {
    auto separator = FileUtilities::getSeparator();
    
    std::string path;
    
    if (type == FileUtilities::PathType::builtin) {
        path += FileUtilities::getBuiltinPath();
    } else {
        path += FileUtilities::getRuntimePath();
    }
    
    if (!subpath.empty()) {
        assert(subpath.compare(0, 1, separator) != 0);
        assert(subpath.compare(subpath.length() - 1, 1, separator) != 0);
        
        path += separator + subpath;
    }
    
    if (!basename.empty()) {
        assert(basename.compare(0, 1, separator) != 0);
        assert(basename.compare(basename.length() - 1, 1, separator) != 0);
        
        path += separator + basename;
    }
    
    return path;
}

std::string BGE::FilePath::extension() const {
    size_t lastIndex = basename.find_last_of(".");
    
    if (lastIndex != std::string::npos) {
        return basename.substr(lastIndex+1);
    }
    
    return "";
}

