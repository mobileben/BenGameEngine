//
//  HandleService.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 4/29/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef HandleService_h
#define HandleService_h

#include <stdio.h>
#include "Service.h"
#include "Handle.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <type_traits>

namespace BGE {
    static const uint32_t HandleServiceNoMaxLimit = 0;
    
    extern void handleServicesInitialize();
    extern void handleServicesLock();
    extern void handleServicesUnlock();
    
    template <typename DATA, typename HANDLE>
    class HandleService : public Service {
    public:
        HandleService(uint32_t reserve, uint32_t maxLimit) : numResizes_(0), maxAllocated_(0), maxLimit_(maxLimit) {
#if UNIT_TESTING
            if (reserve == 0) {
                throw std::exception();
            } else if (maxLimit != 0 && maxLimit < reserve) {
                throw std::exception();
            }
#else
            assert(reserve > 0 && (maxLimit == HandleServiceNoMaxLimit || maxLimit >= reserve));
#endif
            data_.reserve(reserve);
            magic_.reserve(reserve);
            freeSlots_.reserve(reserve);
            toBeFreeSlots_.reserve(reserve);
            
            initialCapacity_ = currCapacity_ = (uint32_t) data_.capacity();
        }
        
        HandleService() = delete;
        ~HandleService() {}
        
        void initialize() final {}
        void reset() final {}
        void platformSuspending() final { Service::platformSuspending(); }
        void platformResuming() final { Service::platformResuming(); }
        void pause() final { Service::pause(); }
        void resume() final { Service::resume(); }
        void destroy() final {}
        void update(double deltaTime) final {}

        DATA* allocate(HANDLE& handle) {
            HandleBackingType index;
            
            handleServicesLock();
            
            if (freeSlots_.empty()) {
                // We have no free slots, so create a new one if we are not at our limit
                if (maxLimit_ != HandleServiceNoMaxLimit && data_.size() >= maxLimit_) {
                    handle.nullify();
                    handleServicesUnlock();

                    return nullptr;
                }
                
                index = (HandleBackingType) magic_.size();
                handle.init(index);
                
                data_.push_back(DATA());
                magic_.push_back(handle.getMagic());
                
                if (data_.size() > maxAllocated_) {
                    maxAllocated_ = (uint32_t) data_.size();
                }
                
                if (data_.capacity() != currCapacity_) {
                    numResizes_++;

#if DEBUG
                    printf("WARNING: %s increased capacity from %u to %lu. Increase #%u\n", typeid(DATA).name(), currCapacity_, data_.capacity(), numResizes_);
#endif
                    
                    currCapacity_ = static_cast<uint32_t>(data_.capacity());
                }
            } else {
                index = freeSlots_.back();
                handle.init(index);
                freeSlots_.pop_back();
                magic_[index] = handle.getMagic();
            }
            
            auto data = &data_[index];
            ObjectId objId = getIdAndIncrement();

            data->setInstanceId(objId);
            
            handleServicesUnlock();
            
            return data;
        }
        
        void release(HANDLE handle) {
            if (handle.isNull()) {
                return;
            }
            
            auto index = handle.getIndex();

            handleServicesLock();

            if (magic_[index] == 0) {
                handleServicesUnlock();
                return;
            }
            
#if UNIT_TESTING
            if (index >= data_.size()) {
                std::exception();
            } else if (magic_[index] != handle.getMagic()) {
                std::exception();
            }
#else
            assert(index < data_.size());
            assert(magic_[index] == handle.getMagic());
#endif
            magic_[index] = 0;
            toBeFreeSlots_.push_back(index);
            
            handleServicesUnlock();
        }
        
        DATA *dereference(HANDLE handle) const {
            if (handle.isNull()) {
                return nullptr;
            }
            
            auto index = handle.getIndex();
            
            handleServicesLock();
            
            if (index < data_.size()) {
                auto magicIndex = magic_[index];
                
                if (magicIndex != handle.getMagic() || magicIndex == 0) {
                    handleServicesUnlock();
                    return nullptr;
                }
            } else {
                handleServicesUnlock();
                return nullptr;
            }
            
            auto retVal = const_cast<DATA *>(&data_[index]);
            
            handleServicesUnlock();
            return retVal;
        }

        void garbageCollect() {
            freeSlots_.insert(freeSlots_.end(), toBeFreeSlots_.begin(), toBeFreeSlots_.end());
            toBeFreeSlots_.clear();
        }
        
        std::vector<DATA *> activePointers() const {
            std::vector<DATA *> pointers;
            handleServicesLock();
            
            for (auto i=0;i<data_.size();i++) {
                if (magic_[i] != 0) {
                    DATA *ptr = const_cast<DATA *>(&data_[i]);
                    
                    pointers.push_back(ptr);
                }
            }
            
            handleServicesUnlock();
            
            return pointers;
        }
        
        uint32_t numUsedHandles() const {
            return (uint32_t) (magic_.size() - freeSlots_.size() - toBeFreeSlots_.size());
        }
        
        uint32_t capacity() const {
            return (uint32_t) data_.capacity();
        }
        
        uint32_t initialCapacity() const {
            return initialCapacity_;
        }
        
        uint32_t numResizes() const {
            return numResizes_;
        }
        
        size_t usedMemory() const {
            return numUsedHandles() * sizeof(DATA);
        }
        
        size_t unusedMemory() const {
            return capacity() * sizeof(DATA) - usedMemory();
        }
        
        size_t totalMemory() const {
            return capacity() * sizeof(DATA);
        }
        
        uint32_t getNumResizes() const {
            return numResizes_;
        }
        
        uint32_t getMaxAllocated() const {
            return maxAllocated_;
        }
        
#if UNIT_TESTING
        // Provide means to get the data from a specific index
        DATA *dataAtIndex(uint32_t index) {
            return &data_[index];
        }
        
        uint32_t magicAtIndex(uint32_t index) {
            return magic_[index];
        }
        
        uint32_t freeIndexAtIndex(uint32_t index) {
            return freeSlots_[index];
        }
        
        uint32_t numFreeSlots() const {
            return (uint32_t) freeSlots_.size();
        }
        
        uint32_t toBeFreeIndexAtIndex(uint32_t index) {
            return toBeFreeSlots_[index];
        }
        
        uint32_t numToBeFreeSlots() const {
            return (uint32_t) toBeFreeSlots_.size();
        }
#endif
        
    private:
        typedef std::vector<DATA> DataVector;
        typedef std::vector<HandleBackingType> MagicVector;
        typedef std::vector<HandleBackingType> FreeVector;
        typedef std::vector<HandleBackingType> ToBeFreeVector;
        
        uint32_t        initialCapacity_;
        uint32_t        currCapacity_;
        uint32_t        numResizes_;
        uint32_t        maxAllocated_;
        uint32_t        maxLimit_;
        DataVector      data_;
        MagicVector     magic_;
        FreeVector      freeSlots_;
        ToBeFreeVector  toBeFreeSlots_;
    };
}

#endif /* HandleService_h */
