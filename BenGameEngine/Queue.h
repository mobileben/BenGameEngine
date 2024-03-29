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
        while (running_ && queue_.empty()) {
            cond_.wait(mlock);
        }
        
        if (running_ && queue_.size()) {
            auto item = queue_.front();
            queue_.pop();
            return item;
        } else {
            return T();
        }
    }
    
    void pop(T& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (running_ && queue_.empty()) {
            cond_.wait(mlock);
        }
        if (running_ && queue_.size()) {
            item = queue_.front();
            queue_.pop();
        } else {
            item = T();
        }
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

    void pushIfEmpty(const T& item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        bool notify = false;
        if (queue_.empty()) {
            queue_.push(item);
            notify = true;
        }
        mlock.unlock();
        if (notify) {
            cond_.notify_one();
        }
    }

    void pushIfEmpty(T&&item) {
        std::unique_lock<std::mutex> mlock(mutex_);
        bool notify = false;
        if (queue_.empty()) {
            queue_.push(item);
            notify = true;
        }
        mlock.unlock();
        if (notify) {
            cond_.notify_one();
        }
    }

    bool empty() {
        std::unique_lock<std::mutex> mlock(mutex_);
        bool e = false;
        e = queue_.empty();
        mlock.unlock();
        return e;
    }

    size_t size() {
        std::unique_lock<std::mutex> mlock(mutex_);
        size_t count = 0;
        count = queue_.size();
        mlock.unlock();
        return count;
    }

    void quit() {
        std::unique_lock<std::mutex> mlock(mutex_);
        running_ = false;
        mlock.unlock();
        cond_.notify_one();
    }
    
    Queue() : running_(true) {}
    Queue(const Queue &) = delete;
    Queue &operator=(const Queue &) = delete;
    
private:
    bool                    running_;
    std::queue<T>           queue_;
    std::mutex              mutex_;
    std::condition_variable cond_;
};

#endif /* Queue_h */
