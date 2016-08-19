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
    template <typename DATA, typename HANDLE>
    class HandleService : public Service {
    public:
        static const uint32_t NoMaxLimit = 0;
        
        HandleService(uint32_t reserve, uint32_t maxLimit) : maxLimit_(maxLimit) {
#if UNIT_TESTING
            if (reserve == 0) {
                throw std::exception();
            } else if (maxLimit != 0 && maxLimit < reserve) {
                throw std::exception();
            }
#else
            assert(reserve > 0 && (maxLimit == 0 || maxLimit >= reserve));
#endif
            
            data_.reserve(reserve);
            magic_.reserve(reserve);
            freeSlots_.reserve(reserve);
        }
        
        HandleService() = delete;
        ~HandleService() {}
        
        static HandleService<DATA, HANDLE> *createService(uint32_t reserve, uint32_t maxLimit) {
            return new HandleService<DATA, HANDLE>(reserve, maxLimit);
        }
        
        void initialize() {}
        void reset() {}
        void enteringBackground() {}
        void enteringForeground() {}
        void pause() {}
        void resume() {}
        void destroy() {}
        void update(double deltaTime) {}

        DATA* allocate(HANDLE& handle) {
            HandleBackingType index;
            
            if (freeSlots_.empty()) {
                // We have no free slots, so create a new one if we are not at our limit
                if (maxLimit_ && data_.size() >= maxLimit_) {
                    handle.nullify();
                    return nullptr;
                }
                
                index = (HandleBackingType) magic_.size();
                handle.init(index);
                
                data_.push_back(DATA());
                magic_.push_back(handle.getMagic());
            } else {
                index = freeSlots_.back();
                handle.init(index);
                freeSlots_.pop_back();
                magic_[index] = handle.getMagic();
            }
            
            auto data = &data_[index];
            ObjectId objId = getIdAndIncrement();

            data->setInstanceId(objId);
            return data;
        }
        
        void release(HANDLE handle) {
            if (handle.isNull()) {
                return;
            }
            
            auto index = handle.getIndex();
            
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
            freeSlots_.push_back(index);
        }
        
        DATA* dereference(HANDLE handle) {
            if (handle.isNull()) {
                return nullptr;
            }
            
            auto index = handle.getIndex();
            
            if (index >= data_.size() || magic_[index] != handle.getMagic()) {
                return nullptr;
            }
            
            return &data_[index];
        }
        
        const DATA* dereference(HANDLE handle) const {
            return (const_cast<HandleService<DATA, HANDLE>*>(this)->dereference(handle));
        }
        
        uint32_t numUsedHandles() const {
            return (uint32_t) (magic_.size() - freeSlots_.size());
        }
        
        uint32_t capacity() const {
            return (uint32_t) data_.capacity();
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
#endif
        
    private:
        typedef std::vector<DATA> DataVector;
        typedef std::vector<HandleBackingType> MagicVector;
        typedef std::vector<HandleBackingType> FreeVector;
        
        uint32_t    maxLimit_;
        DataVector  data_;
        MagicVector magic_;
        FreeVector  freeSlots_;
    };
}

#endif /* HandleService_h */
