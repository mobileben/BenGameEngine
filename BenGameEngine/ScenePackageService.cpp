//
//  ScenePackageService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ScenePackageService.h"
#import <Foundation/Foundation.h>

BGE::ScenePackageService::ScenePackageService() : scenePackageHandleService_(InitialScenePackageReserve, ScenePackageHandleService::NoMaxLimit) {
}

void BGE::ScenePackageService::packageFromJSONFile(std::string filename, std::string name, std::function<void(ScenePackageHandle, std::shared_ptr<BGE::Error>)> callback) {
    ScenePackagesMapIterator it = scenePackages_.find(name);
    
    if (it != scenePackages_.end()) {
        if (callback) {
            callback(it->second, nullptr);
        }
    } else {
        // Does not exist, to load
        // For now we are doing this via iOS methods for faster dev
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            NSFileManager *defaultMgr = [NSFileManager defaultManager];
            ScenePackageHandle handle;
            ScenePackage *package = scenePackageHandleService_.allocate(handle);
            
            if (package) {
                NSData *data = [defaultMgr contentsAtPath:[[NSString alloc] initWithCString:filename.c_str() encoding:NSUTF8StringEncoding]];
                NSError *err = nil;
                NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&err];

                package->load(jsonDict, [handle, callback](ScenePackage *) {
                    if (callback) {
                        callback(handle, nullptr);
                    }
                });
            }
            
            
            if (callback) {
                callback(handle, nullptr);
            }
        });
    }
}

void BGE::ScenePackageService::getPackage(std::string name) {
}

void BGE::ScenePackageService::deletePackage(std::string name) {
}

void BGE::ScenePackageService::resetPackage(std::string name) {
}
