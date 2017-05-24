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
    
    const SignalId UnassignedSignalId = -1;
    
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
            if (id == UnassignedSignalId) {
                return;
            }
            slots_.erase(id);
        }
        
        void disconnectAll() const {
            std::lock_guard<std::mutex> lock(mutex_);
            slots_.clear();
        }
        
        void emit(Args... args) {
            mutex_.lock();

            // Make a copy of what we need to emit against
            // Any calls to connect or disconnect done by any of the functions called will NOT be affected by this emit
            auto slotsCopy = slots_;
            
            mutex_.unlock();
            
            for (auto& it : slotsCopy) {
                if (!it.second(args...)) {
                    // This should not be a common occurence
                    mutex_.lock();
                    slots_.erase(it.first);
                    mutex_.unlock();
                }
            }
        }
        
    private:
        using SignalMap = std::unordered_map<SignalId, std::function<bool(Args...)>>;
        
        mutable std::mutex  mutex_;
        mutable SignalId    currentId_;
        mutable SignalMap   slots_;
    };
}

#endif /* Observer_h */
