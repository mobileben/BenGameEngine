//
//  Queue.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 8/27/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//
//  Queue comes from here
//  https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/
//  https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
//

#ifndef Queue_h
#define Queue_h

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue
{
public:
    T pop() {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        
        auto item = queue_.front();
        queue_.pop();
        return item;
    }
    
    void pop(T& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
    }
    
    void push(const T& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }
    
    void push(T&& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(std::move(item));
        mlock.unlock();
        cond_.notify_one();
    }
    
    bool empty() const {
        bool e = false;
        
        std::unique_lock<std::mutex> mlock(mutex_);
        
        e = queue_.empty();
        
        mlock.unlock();
        
        return e;
    }
    
    Queue() = default;
    Queue(const Queue &) = delete;
    Queue &operator=(const Queue &) = delete;
    
private:
    std::queue<T>           queue_;
    std::mutex              mutex_;
    std::condition_variable cond_;
};

#endif /* Queue_h */
