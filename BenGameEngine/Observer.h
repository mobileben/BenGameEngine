//
//  Observer.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 5/22/17.
//  Copyright © 2017 2n Productions. All rights reserved.
//

#ifndef Observer_h
#define Observer_h

#include <cstdint>

#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace BGE {
    using SignalId = int64_t;
    
    template <typename... Args>
    class Signal {
    public:
        Signal() : currentId_(0) {}
        Signal(const Signal& other) = delete;

        Signal& operator=(const Signal& other) = delete;

        // functions added to slots must not connnect
        template<typename T>
        SignalId connect(T *obj, bool (T::*func)(Args...)) {
            return connect([=](Args... args) {
                (obj->*func)(args...);
            });
        }
        
        // functions added to slots must not connnect
        template<typename T>
        SignalId connect(T *obj, bool (T::*func)(Args...) const){
            return connect([=](Args... args) {
                (obj->*func)(args...);
            });
        }
        
        SignalId connect(const std::function<bool(Args...)>& slot) const {
            std::lock_guard<std::mutex> lock(mutex_);
            slots_[++currentId_] = slot;
            return currentId_;
        }
        
        void disconnect(SignalId id) const {
            std::lock_guard<std::mutex> lock(mutex_);
            slots_.erase(id);
        }
        
        void disconnectAll() const {
            std::lock_guard<std::mutex> lock(mutex_);
            slots_.clear();
        }
        
        void emit(Args... args) {
            mutex_.lock();
            
            auto slotsCopy = slots_;
            std::vector<SignalId> toRemove;
            
            mutex_.unlock();
            
            for (auto& it : slotsCopy) {
                if (!it.second(args...)) {
                    toRemove.push_back(it.first);
                }
            }
 
            mutex_.lock();
            
            for (auto id : toRemove) {
                slots_.erase(id);
            }
            
            mutex_.unlock();
        }
        
    private:
        using SignalMap = std::unordered_map<SignalId, std::function<bool(Args...)>>;
        
        mutable std::mutex  mutex_;
        mutable SignalId    currentId_;
        mutable SignalMap   slots_;
    };
}

#endif /* Observer_h */
