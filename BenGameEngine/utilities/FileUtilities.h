//
//  FileUtilities.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 9/8/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef FileUtilities_h
#define FileUtilities_h

#include <stdio.h>
#include <string>

namespace BGE {
    struct BaseDirectory;
    struct FilePath;
    
    class FileUtilities {
    public:
        enum class PathType {
            builtin,
            download
        };
        
        static std::string getDownloadPath();
        static std::string getBuiltinPath();
        
        static std::string getPath(const BaseDirectory &base);
        static std::string getPath(const FilePath &filePath);
        
        inline static std::string getSeparator() { return separator_; }
        
        static void initialize(std::string builtinPath, std::string downloadPath, std::string separator="/");
        
    private:
        static bool initialized_;
        static std::string downloadPath_;
        static std::string builtinPath_;
        static std::string separator_;
    };
    
    struct BaseDirectory {
        FileUtilities::PathType type;
        std::string             subpath;
        
        BaseDirectory() : type(FileUtilities::PathType::builtin) {}
        BaseDirectory(FileUtilities::PathType t, std::string path) : type(t), subpath(path) {}
        BaseDirectory(std::string path);
        
        std::string filename() const;
    };
    
    struct FilePath {
        FileUtilities::PathType type;
        std::string             subpath;
        std::string             basename;
        
        FilePath() : type(FileUtilities::PathType::builtin) {}
        FilePath(FileUtilities::PathType t, std::string path) : type(t), subpath(path) {}
        FilePath(FileUtilities::PathType t, std::string path, std::string name) : type(t), subpath(path), basename(name) {}
        FilePath(const BaseDirectory &base) : type(base.type), subpath(base.subpath) {}
        
        std::string filename() const;
        std::string extension() const;
    };
}

#endif /* FileUtilities_h */
