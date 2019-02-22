//
//  ScenePackageService.cpp
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/12/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "ScenePackageService.h"
#include "Game.h"
#include <future>
#include <functional>

#if DEBUG
#include <pthread.h>
#endif

BGE::ScenePackageService::ScenePackageService() : handleService_(InitialScenePackageReserve, HandleServiceNoMaxLimit) {
    loadThread_ = std::thread(&ScenePackageService::loadThreadFunction, this);
}

uint32_t BGE::ScenePackageService::numUsedHandles() const {
    return handleService_.numUsedHandles();
}

uint32_t BGE::ScenePackageService::maxHandles() const {
    return handleService_.capacity();
}

uint32_t BGE::ScenePackageService::numHandleResizes() const {
    return handleService_.numResizes();
}

uint32_t BGE::ScenePackageService::maxHandlesAllocated() const {
    return handleService_.getMaxAllocated();
}

size_t BGE::ScenePackageService::usedHandleMemory() const {
    return handleService_.usedMemory();
}

size_t BGE::ScenePackageService::unusedHandleMemory() const {
    return handleService_.unusedMemory();
}

size_t BGE::ScenePackageService::totalHandleMemory() const {
    return handleService_.totalMemory();
}

size_t BGE::ScenePackageService::totalMemory() const {
    size_t total = 0;
    
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            total += package->getMemoryUsage();
        }
    }
    
    return total;
}

uint32_t BGE::ScenePackageService::numScenePackages() const {
    uint32_t num = 0;
    
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            num++;
        }
    }
    
    return num;
}

bool BGE::ScenePackageService::isAligned8Memory(size_t size) const {
    return !(size&7);
}

size_t BGE::ScenePackageService::aligned8MemorySize(size_t size) const {
    size_t fixed = size;
    if (!isAligned8Memory(size)) {
        size_t tmp = size / 8;
        fixed = (tmp + 1) * 8;
    }
    return fixed;
}

void BGE::ScenePackageService::loadPackageTextures(ScenePackageLoadItem loadable, __attribute__ ((unused)) ScenePackageLoadCompletionHandler callback) {
    if (loadable.type == ScenePackageLoadItem::LoadType::Textures) {
        auto package = getScenePackage(loadable.packageHandle);
        if (package) {
            package->loadAllTextures([loadable]() {
                if (loadable.completionHandler) {
                    loadable.completionHandler(loadable.packageHandle, nullptr);
                }
            });
        } else if (loadable.completionHandler) {
            loadable.completionHandler(loadable.packageHandle, std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
        }
    } else if (loadable.completionHandler) {
        loadable.completionHandler(loadable.packageHandle, std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
    }
}


void BGE::ScenePackageService::dispatchAsync(ScenePackageLoadCompletionHandler func) {
    ScenePackageLoadItem loadable(func);
    queuedLoadItems_.push(loadable);
}

void BGE::ScenePackageService::createPackage(SpaceHandle spaceHandle, std::string name, const FilePath &filePath, ScenePackageLoadCompletionHandler callback) {
    ScenePackageLoadItem loadable{spaceHandle, name, filePath, callback};
    queuedLoadItems_.push(loadable);
}

void BGE::ScenePackageService::createPackage(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    for (auto &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getName() == loadable.name) {
            // Check if our spaceHandle is in the refernce list, if not add it
            bool found = false;
            
            for (auto const &ref : packageRef.references) {
                if (ref == loadable.spaceHandle) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                auto space = Game::getInstance()->getSpaceService()->getSpace(loadable.spaceHandle);
                if (space) {
                    space->scenePackageAdded(packageRef.handle);
                }
                packageRef.references.push_back(loadable.spaceHandle);
            }
            
            if (callback) {
                callback(packageRef.handle, nullptr);
            }
            return;
        }
    }
    
    auto ext = loadable.filePath.extension();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        createPackageFromJSON(loadable, callback);
    } else if (ext == "spkg") {
        createPackageFromSPKG(loadable, callback);
    } else {
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::UnsupportedFormat)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromSPKGBinary(SpaceHandle spaceHandle, std::string name, const uint64_t *buffer, size_t bufferSize, bool managed, const BaseDirectory& baseDirectory, ScenePackageLoadCompletionHandler callback) {
    if (buffer) {
        uint64_t *packageBuffer = const_cast<uint64_t *>(buffer);
        if (!managed) {
            bufferSize = aligned8MemorySize(bufferSize);
            packageBuffer = new uint64_t[bufferSize / sizeof(uint64_t)];
            memcpy(packageBuffer, buffer, bufferSize);
        }

        ScenePackageHandle handle;
        ScenePackage *package = handleService_.allocate(handle);

        if (package) {
            package->initialize(handle, name);
            package->setBaseDirectory(baseDirectory);
            package->setStatus(ScenePackageStatus::Loading);

            // Now create an entry for this
            ScenePackageReference ref{ handle, { spaceHandle }};
            scenePackages_.push_back(ref);

            // The loading system relies on having the package initalized with handle
            package->create(packageBuffer, bufferSize, managed, [this, spaceHandle, callback, handle](ScenePackage * package) {
                auto tHandle = handle;
                std::shared_ptr<Error> error = nullptr;

                if (package) {
                    package->setStatus(ScenePackageStatus::Valid);

                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);

                    if (space) {
                        space->scenePackageAdded(tHandle);
                    }
                } else {
                    error = std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::Loading));

                    // There is no package, so we need to release this
                    removePackage(spaceHandle, tHandle);

                    // Reset handle
                    tHandle.nullify();
                }

                if (callback) {
                    callback(tHandle, error);
                }
            });
        } else {
            if (!managed) {
                delete [] packageBuffer;
            }

            if (callback) {
                callback(handle, std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::NoAvailableHandles)));
            }
        }
    }
}

void BGE::ScenePackageService::createPackageFromJSONBinary(SpaceHandle spaceHandle, std::string name, const uint8_t *buffer, size_t bufferSize, const BaseDirectory &baseDirectory, ScenePackageLoadCompletionHandler callback) {
    rapidjson::MemoryStream memStream((const rapidjson::MemoryStream::Ch *)buffer, bufferSize);
    rapidjson::AutoUTFInputStream<unsigned, rapidjson::MemoryStream> is(memStream);
    std::shared_ptr<rapidjson::Document> doc = std::make_shared<rapidjson::Document>();
    doc->ParseStream(is);
    if (!doc->HasParseError()) {
        BGE::Game::getInstance()->getScenePackageService()->createPackageFromJSONDict(spaceHandle, name, doc, baseDirectory, callback);
    } else if (callback) {
        callback(BGE::ScenePackageHandle(), std::make_shared<BGE::Error>(BGE::ScenePackage::ErrorDomain, static_cast<int32_t>(BGE::ScenePackageError::DoesNotExist)));
    }
}

void BGE::ScenePackageService::createPackageFromJSONDict(SpaceHandle spaceHandle, std::string name, const std::shared_ptr<rapidjson::Document> jsonDict, const BaseDirectory &baseDirectory, ScenePackageLoadCompletionHandler callback) {
    if (jsonDict) {
        ScenePackageHandle handle;
        ScenePackage *package = handleService_.allocate(handle);

        if (package) {
            package->initialize(handle, name);
            package->setBaseDirectory(baseDirectory);
            package->setStatus(ScenePackageStatus::Loading);

            // Now create an entry for this
            ScenePackageReference ref{ handle, { spaceHandle }};
            scenePackages_.push_back(ref);

            // The loading system relies on having the package initalized with handle
            package->create(jsonDict, [this, spaceHandle, callback, handle](ScenePackage * package) {
                auto tHandle = handle;
                std::shared_ptr<Error> error = nullptr;

                if (package) {
                    package->setStatus(ScenePackageStatus::Valid);

                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);

                    if (space) {
                        space->scenePackageAdded(tHandle);
                    }
                } else {
                    error = std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::Loading));

                    // There is no package, so we need to release this
                    removePackage(spaceHandle, tHandle);

                    // Reset handle
                    tHandle.nullify();
                }

                if (callback) {
                    callback(tHandle, error);
                }
            });
        } else {
            if (callback) {
                callback(handle, std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::NoAvailableHandles)));
            }
        }
    } else {
        if (callback) {
            callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
        }
    }
}

void BGE::ScenePackageService::createPackageFromJSON(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    FILE *fp = fopen(loadable.filePath.filename().c_str(), "rb");
    if (fp) {
        fseek(fp, 0L, SEEK_END);
        auto bufferSize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        auto buffer = new uint8_t[bufferSize];
        fread(buffer, sizeof(uint8_t), bufferSize, fp);
        fclose(fp);

        createPackageFromJSONBinary(loadable.spaceHandle, loadable.name, buffer, bufferSize, BaseDirectory{loadable.filePath.type, loadable.filePath.subpath}, callback);
        delete [] buffer;
    } else if (callback) {
        callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
    }
}

void BGE::ScenePackageService::createPackageFromSPKG(ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    FILE *fp = fopen(loadable.filePath.filename().c_str(), "rb");
    if (fp) {
        // We want 64-bit aligned memory
        fseek(fp, 0L, SEEK_END);
        auto bufferSize = aligned8MemorySize(ftell(fp));
        fseek(fp, 0L, SEEK_SET);
        auto buffer = new uint64_t[bufferSize / sizeof(uint64_t)];
        fread(buffer, sizeof(uint8_t), bufferSize, fp);
        fclose(fp);

        createPackageFromSPKGBinary(loadable.spaceHandle, loadable.name, buffer, bufferSize, false, BaseDirectory{loadable.filePath.type, loadable.filePath.subpath}, callback);
        delete [] buffer;
    } else if (callback) {
        callback(ScenePackageHandle(), std::make_shared<Error>(ScenePackage::ErrorDomain, static_cast<int32_t>(ScenePackageError::DoesNotExist)));
    }
}

void BGE::ScenePackageService::completionPackage(__attribute__ ((unused)) ScenePackageLoadItem loadable, ScenePackageLoadCompletionHandler callback) {
    if (callback) {
        callback(ScenePackageHandle(), nullptr);
    }
}

BGE::ScenePackageHandle BGE::ScenePackageService::getScenePackageHandle(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package && package->getName() == name) {
            return package->getHandle();
        }
    }
    
    return ScenePackageHandle();
}


void BGE::ScenePackageService::addSpaceHandleReference(SpaceHandle spaceHandle, ScenePackageHandle packageHandle) {
    for (auto &pkgRef : scenePackages_) {
        if (pkgRef.handle == packageHandle) {
            bool found = false;

            for (auto &ref : pkgRef.references) {
                if (ref == spaceHandle) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                pkgRef.references.push_back(spaceHandle);
                
                auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                
                space->scenePackageAdded(packageHandle);
            }
            break;
        }
    }
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(std::string name) const {
    auto handle = getScenePackageHandle(name);
    
    return getScenePackage(handle);
}

BGE::ScenePackage *BGE::ScenePackageService::getScenePackage(ScenePackageHandle handle) const {
    return handleService_.dereference(handle);
}

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, std::string name) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        auto package = getScenePackage(it->handle);
        
        if (package && package->getName() == name) {
            removePackage(spaceHandle, it->handle);
        }
    }
}

void BGE::ScenePackageService::removePackage(SpaceHandle spaceHandle, ScenePackageHandle handle) {
    for (auto it=scenePackages_.begin(); it!=scenePackages_.end(); ++it) {
        if (it->handle == handle) {
            auto package = getScenePackage(handle);
            auto &refs = it->references;

            for (auto hIt=refs.begin(); hIt!=refs.end();++hIt) {
                if (*hIt == spaceHandle) {
                    auto space = Game::getInstance()->getSpaceService()->getSpace(spaceHandle);
                    
                    if (space) {
                        space->scenePackageRemoved(package->getHandle());
                    }
                    refs.erase(hIt);
                    break;
                }
            }
            
            // We have no more references, so delete the scene
            if (refs.size() == 0) {
                releasePackage(package);
                scenePackages_.erase(it);
            }
            
            break;
        }
    }
}

void BGE::ScenePackageService::resetPackage(__attribute__ ((unused)) std::string name) {
}

void BGE::ScenePackageService::resetPackage(__attribute__ ((unused)) ScenePackageHandle handle) {
    
}

void BGE::ScenePackageService::releasePackage(ScenePackage *package) {
    if (package) {
        auto handle = package->getHandle();
        package->destroy();
        handleService_.release(handle);
    }
}

void BGE::ScenePackageService::link() {
    // TODO: Build dependency lists
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);

        if (package) {
            package->link();
        }
    }
}

BGE::ButtonReference *BGE::ScenePackageService::getButtonReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto button = package->getButtonReference(name);
            
            if (button) {
                return button;
            }
        }
    }
    
    return nullptr;
}

BGE::MaskReference *BGE::ScenePackageService::getMaskReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto mask = package->getMaskReference(name);
            
            if (mask) {
                return mask;
            }
        }
    }
    
    return nullptr;
}

BGE::TextureMaskReference *BGE::ScenePackageService::getTextureMaskReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto mask = package->getTextureMaskReference(name);
            
            if (mask) {
                return mask;
            }
        }
    }
    
    return nullptr;
}

BGE::PlacementReference *BGE::ScenePackageService::getPlacementReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto placement = package->getPlacementReference(name);
            
            if (placement) {
                return placement;
            }
        }
    }
    
    return nullptr;
}

BGE::TextureReference *BGE::ScenePackageService::getTextureReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto texture = package->getTextureReference(name);
            
            if (texture) {
                return texture;
            }
        }
    }
    
    return nullptr;
}

BGE::TextReference *BGE::ScenePackageService::getTextReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto text = package->getTextReference(name);
            
            if (text) {
                return text;
            }
        }
    }
    
    return nullptr;
}

BGE::AnimationSequenceReference *BGE::ScenePackageService::getAnimationSequenceReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto animSeq = package->getAnimationSequenceReference(name);
            
            if (animSeq) {
                return animSeq;
            }
        }
    }
    
    return nullptr;
}

BGE::ExternalPackageReference *BGE::ScenePackageService::getExternalReference(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto extRef = package->getExternalReference(name);
            
            if (extRef) {
                return extRef;
            }
        }
    }
    
    return nullptr;
}

BGE::GfxReferenceType BGE::ScenePackageService::getReferenceType(std::string name) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            auto type = package->getReferenceType(name);
            
            if (type != GfxReferenceTypeUnknown) {
                return type;
            }
        }
    }

    return GfxReferenceTypeUnknown;
}

void BGE::ScenePackageService::loadThreadFunction() {
    auto native = loadThread_.native_handle();
    if (native == pthread_self()) {
        pthread_setname_np("scenepackage");
    }
    while (true) {
        auto loadable = queuedLoadItems_.pop();

        if (loadable.type == ScenePackageLoadItem::LoadType::Package) {
            auto f = std::async(std::launch::async, static_cast<void(ScenePackageService::*)(ScenePackageLoadItem, ScenePackageLoadCompletionHandler)>(&ScenePackageService::createPackage), this, loadable, [loadable](ScenePackageHandle packageHandle, std::shared_ptr<Error> error) {

                if (loadable.completionHandler) {
                    loadable.completionHandler(packageHandle, error);
                }
            });
        } else if (loadable.type == ScenePackageLoadItem::LoadType::Textures) {
            auto f = std::async(std::launch::async, static_cast<void(ScenePackageService::*)(ScenePackageLoadItem, ScenePackageLoadCompletionHandler)>(&ScenePackageService::loadPackageTextures), this, loadable, [loadable](ScenePackageHandle packageHandle, std::shared_ptr<Error> error) {

                if (loadable.completionHandler) {
                    loadable.completionHandler(packageHandle, error);
                }
            });
        } else if (loadable.type == ScenePackageLoadItem::LoadType::Function) {
            auto f = std::async(std::launch::async, static_cast<void(ScenePackageService::*)(ScenePackageLoadItem, ScenePackageLoadCompletionHandler)>(&ScenePackageService::completionPackage), this, loadable, [loadable](ScenePackageHandle packageHandle, std::shared_ptr<Error> error) {
                if (loadable.completionHandler) {
                    loadable.completionHandler(packageHandle, error);
                }
            });
        }
    }
}

void BGE::ScenePackageService::outputResourceBreakdown(uint32_t numTabs) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            package->outputResourceBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

void BGE::ScenePackageService::outputMemoryBreakdown(uint32_t numTabs) const {
    for (auto const &packageRef : scenePackages_) {
        auto package = getScenePackage(packageRef.handle);
        
        if (package) {
            package->outputMemoryBreakdown(numTabs);
        } else {
            assert(false);
        }
    }
}

